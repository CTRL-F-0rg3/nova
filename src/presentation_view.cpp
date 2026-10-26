#include "presentation_view.hpp"
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebChannel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QImage>
#include <QPainter>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class BridgeObject : public QObject {
    Q_OBJECT
public:
    int selectedIndex = -1;

signals:
    void imageGeomChanged(int idx, double x, double y, double w, double h);
    void imageSelected(int idx);
    void imageDropped(const QString& path, double x, double y);
    void contentUpdated(const QString& title, const QString& subtitle, const QString& bodyHtml);

public slots:
    Q_INVOKABLE void updateImageGeom(int idx, double x, double y, double w, double h) {
        emit imageGeomChanged(idx, x, y, w, h);
    }
    Q_INVOKABLE void setSelected(int idx) {
        selectedIndex = idx;
        emit imageSelected(idx);
    }
    Q_INVOKABLE void updateContent(const QString& title, const QString& subtitle, const QString& bodyHtml) {
        emit contentUpdated(title, subtitle, bodyHtml);
    }
};

class DropWebView : public QWebEngineView {
    Q_OBJECT
public:
    explicit DropWebView(QWidget* parent = nullptr) : QWebEngineView(parent) {
        setAcceptDrops(true);
    }
signals:
    void imageDropped(const QString& path, double x, double y);
protected:
    void dragEnterEvent(QDragEnterEvent* e) override {
        if (e->mimeData()->hasUrls()) {
            for (const auto& u : e->mimeData()->urls()) {
                QString s = u.toLocalFile().toLower();
                if (s.endsWith(".png") || s.endsWith(".jpg") || s.endsWith(".jpeg") ||
                    s.endsWith(".bmp") || s.endsWith(".gif") || s.endsWith(".webp")) {
                    e->acceptProposedAction();
                    return;
                }
            }
        }
        e->ignore();
    }
    void dropEvent(QDropEvent* e) override {
        QPointF pos = e->position();
        double x = pos.x() / width();
        double y = pos.y() / height();
        for (const auto& u : e->mimeData()->urls()) {
            QString s = u.toLocalFile();
            if (!s.isEmpty()) { emit imageDropped(s, x, y); break; }
        }
        e->acceptProposedAction();
    }
};

static const char* kHtmlTemplate = R"HTML(<!DOCTYPE html>
<html><head><meta charset="utf-8"><style>
* { box-sizing: border-box; margin: 0; padding: 0; }
html, body { width: 100%; height: 100%; overflow: hidden; }
body { font-family: 'Helvetica', 'Arial', sans-serif; }
.slide { position: relative; width: 100vw; height: 100vh; overflow: hidden; }
.content {
  position: absolute; left: 8%; top: 8%; width: 84%; height: 84%;
  overflow-y: auto; outline: none;
}
.content h1 { font-size: 3em; margin-bottom: 0.3em; outline: none; }
.content h2 { font-size: 2em; margin-bottom: 0.5em; outline: none; }
.content h3 { font-size: 1.5em; margin-bottom: 0.5em; outline: none; }
.content p { font-size: 1.3em; line-height: 1.5; margin-bottom: 0.5em; }
.content ul, .content ol { font-size: 1.3em; line-height: 1.6; margin-left: 1.5em; margin-bottom: 0.5em; }
.content li { margin-bottom: 0.2em; }
.editable { outline: none; }
.editable:empty:before { content: attr(data-ph); color: rgba(255,255,255,0.3); }
.content:focus, .editable:focus { outline: none; }
.image {
  position: absolute; cursor: move; user-select: none;
  border: 0;
}
.image img {
  width: 100%; height: 100%; object-fit: cover;
  pointer-events: none; display: block;
}
.image.selected { outline: 2px solid #ff2d95; }
.handle {
  position: absolute; width: 10px; height: 10px;
  background: #ff2d95; border: 1px solid white; z-index: 9999;
}
.handle.tl { left: -5px; top: -5px; cursor: nwse-resize; }
.handle.t  { left: calc(50% - 5px); top: -5px; cursor: ns-resize; }
.handle.tr { right: -5px; top: -5px; cursor: nesw-resize; }
.handle.r  { right: -5px; top: calc(50% - 5px); cursor: ew-resize; }
.handle.br { right: -5px; bottom: -5px; cursor: nwse-resize; }
.handle.b  { left: calc(50% - 5px); bottom: -5px; cursor: ns-resize; }
.handle.bl { left: -5px; bottom: -5px; cursor: nesw-resize; }
.handle.l  { left: -5px; top: calc(50% - 5px); cursor: ew-resize; }
.footer {
  position: absolute; right: 20px; bottom: 20px;
  font-size: 12px; opacity: 0.5; pointer-events: none;
}
</style></head><body>
<div class="slide" id="slide">
  <div class="content">
    <h1 class="editable" id="title" contenteditable="true" data-ph="Tytuł"></h1>
    <h2 class="editable" id="subtitle" contenteditable="true" data-ph=""></h2>
    <div class="editable" id="body" contenteditable="true" data-ph="Treść..."></div>
  </div>
  <div class="footer" id="footer"></div>
</div>
<script src="qwebchannel.js"></script>
<script>
var bridge = null;
var slideImages = [];
var selectedIdx = -1;
var dragMode = null;
var dragStart = null;
var dragImgStart = null;
var syncTimer = null;
var initialized = false;

new QWebChannel(qt.webChannelTransport, function(channel) {
  bridge = channel.objects.bridge;
});

function setupListeners() {
  if (initialized) return;
  initialized = true;
  ['title','subtitle','body'].forEach(function(id){
    var el = document.getElementById(id);
    el.addEventListener('input', debounceSync);
    el.addEventListener('blur', syncNow);
  });
}

function debounceSync() {
  clearTimeout(syncTimer);
  syncTimer = setTimeout(syncNow, 400);
}

function syncNow() {
  if (!bridge) return;
  var title = document.getElementById('title').innerText.trim();
  var subtitle = document.getElementById('subtitle').innerText.trim();
  var bodyHtml = document.getElementById('body').innerHTML.trim();
  bridge.updateContent(title, subtitle, bodyHtml);
}

function renderSlide(data) {
  var body = document.body;
  body.style.backgroundColor = data.bgColor || '#050308';
  body.style.color = data.fgColor || '#e0dde8';
  if (data.bgImage) {
    body.style.backgroundImage = 'url("' + data.bgImage + '")';
    body.style.backgroundSize = 'cover';
    body.style.backgroundPosition = 'center';
  } else {
    body.style.backgroundImage = '';
  }

  var title = document.getElementById('title');
  var subtitle = document.getElementById('subtitle');
  var bodyEl = document.getElementById('body');

  if (document.activeElement !== title) title.innerText = data.title || '';
  if (document.activeElement !== subtitle) subtitle.innerText = data.subtitle || '';
  if (document.activeElement !== bodyEl) bodyEl.innerHTML = data.bodyHtml || '';

  document.getElementById('footer').textContent = data.footerText || '';

  document.querySelectorAll('.image').forEach(function(e){ e.remove(); });
  document.querySelectorAll('.handle').forEach(function(e){ e.remove(); });

  slideImages = data.images || [];
  selectedIdx = -1;

  slideImages.forEach(function(img, i) {
    var div = document.createElement('div');
    div.className = 'image';
    div.id = 'img_' + i;
    div.style.left = (img.x * 100) + '%';
    div.style.top = (img.y * 100) + '%';
    div.style.width = (img.w * 100) + '%';
    div.style.height = (img.h * 100) + '%';
    div.style.zIndex = 100 + (img.zOrder || 0);
    var inner = document.createElement('img');
    inner.src = img.path.startsWith('http') ? img.path : 'file://' + img.path;
    inner.style.objectFit = img.preserveAspect ? 'contain' : 'cover';
    div.appendChild(inner);
    div.addEventListener('mousedown', function(e){ onImageMouseDown(e, i); });
    document.getElementById('slide').appendChild(div);
  });

  setupListeners();
}

function onImageMouseDown(e, i) {
  e.preventDefault();
  e.stopPropagation();
  selectImage(i);
  var img = slideImages[i];
  dragStart = { x: e.clientX, y: e.clientY };
  dragImgStart = { x: img.x, y: img.y, w: img.w, h: img.h };
  var target = e.target;
  if (target.classList && target.classList.contains('handle')) {
    dragMode = target.dataset.mode;
  } else {
    dragMode = 'move';
  }
  document.addEventListener('mousemove', onMouseMove);
  document.addEventListener('mouseup', onMouseUp);
}

function onMouseMove(e) {
  if (!dragMode || selectedIdx < 0) return;
  var W = window.innerWidth, H = window.innerHeight;
  var dx = (e.clientX - dragStart.x) / W;
  var dy = (e.clientY - dragStart.y) / H;
  var img = slideImages[selectedIdx];
  var aspect = dragImgStart.w / Math.max(0.001, dragImgStart.h);
  var shift = e.shiftKey;

  if (dragMode === 'move') {
    img.x = Math.max(0, Math.min(1 - dragImgStart.w, dragImgStart.x + dx));
    img.y = Math.max(0, Math.min(1 - dragImgStart.h, dragImgStart.y + dy));
  } else {
    img.x = dragImgStart.x; img.y = dragImgStart.y;
    img.w = dragImgStart.w; img.h = dragImgStart.h;
    if (dragMode.indexOf('t') >= 0) {
      img.y = Math.max(0, dragImgStart.y + dy);
      img.h = Math.max(0.05, dragImgStart.h - dy);
    }
    if (dragMode.indexOf('b') >= 0) {
      img.h = Math.min(1 - img.y, dragImgStart.h + dy);
    }
    if (dragMode.indexOf('l') >= 0) {
      img.x = Math.max(0, dragImgStart.x + dx);
      img.w = Math.max(0.05, dragImgStart.w - dx);
    }
    if (dragMode.indexOf('r') >= 0) {
      img.w = Math.min(1 - img.x, dragImgStart.w + dx);
    }
    if (shift) img.w = img.h * aspect;
  }
  applyImageGeom(selectedIdx);
}

function onMouseUp(e) {
  if (selectedIdx >= 0 && dragMode) {
    var img = slideImages[selectedIdx];
    if (bridge) bridge.updateImageGeom(selectedIdx, img.x, img.y, img.w, img.h);
  }
  dragMode = null;
  document.removeEventListener('mousemove', onMouseMove);
  document.removeEventListener('mouseup', onMouseUp);
}

function applyImageGeom(i) {
  var img = slideImages[i];
  var div = document.getElementById('img_' + i);
  if (!div) return;
  div.style.left = (img.x * 100) + '%';
  div.style.top = (img.y * 100) + '%';
  div.style.width = (img.w * 100) + '%';
  div.style.height = (img.h * 100) + '%';
}

function selectImage(i) {
  selectedIdx = i;
  document.querySelectorAll('.image').forEach(function(e){ e.classList.remove('selected'); });
  document.querySelectorAll('.handle').forEach(function(e){ e.remove(); });
  if (i < 0) { if (bridge) bridge.setSelected(-1); return; }
  var div = document.getElementById('img_' + i);
  if (!div) return;
  div.classList.add('selected');
  ['tl','t','tr','r','br','b','bl','l'].forEach(function(mode){
    var h = document.createElement('div');
    h.className = 'handle ' + mode;
    h.dataset.mode = mode;
    h.addEventListener('mousedown', function(e){ onImageMouseDown(e, i); });
    div.appendChild(h);
  });
  if (bridge) bridge.setSelected(i);
}

function cmdBold() { document.execCommand('bold', false, null); syncNow(); }
function cmdItalic() { document.execCommand('italic', false, null); syncNow(); }
function cmdUnderline() { document.execCommand('underline', false, null); syncNow(); }
function cmdH1() { document.execCommand('formatBlock', false, 'h1'); syncNow(); }
function cmdH2() { document.execCommand('formatBlock', false, 'h2'); syncNow(); }
function cmdH3() { document.execCommand('formatBlock', false, 'h3'); syncNow(); }
function cmdP() { document.execCommand('formatBlock', false, 'p'); syncNow(); }
function cmdUL() { document.execCommand('insertUnorderedList', false, null); syncNow(); }
function cmdOL() { document.execCommand('insertOrderedList', false, null); syncNow(); }

function deleteSelected() {
  if (selectedIdx < 0) return;
  var div = document.getElementById('img_' + selectedIdx);
  if (div) div.remove();
  slideImages.splice(selectedIdx, 1);
  if (bridge) bridge.updateImageGeom(selectedIdx, -1, -1, -1, -1);
  selectedIdx = -1;
  syncNow();
}

document.addEventListener('keydown', function(e) {
  if ((e.key === 'Delete' || e.key === 'Backspace') && selectedIdx >= 0) {
    e.preventDefault();
    deleteSelected();
  }
});
</script></body></html>)HTML";

PresentationView::PresentationView(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_web = new DropWebView(this);
    m_web->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    m_web->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    layout->addWidget(m_web);

    m_bridge = new BridgeObject();
    auto* channel = new QWebChannel(m_web->page());
    channel->registerObject("bridge", m_bridge);
    m_web->page()->setWebChannel(channel);

    connect(m_web, &DropWebView::imageDropped, this, &PresentationView::onBridgeImageDropped);
    connect(m_bridge, &BridgeObject::imageGeomChanged, this, &PresentationView::onBridgeImageGeom);
    connect(m_bridge, &BridgeObject::imageSelected, this, &PresentationView::onBridgeImageSelected);
    connect(m_bridge, &BridgeObject::contentUpdated, this, &PresentationView::onBridgeContent);

    m_web->setHtml(QString::fromUtf8(kHtmlTemplate), QUrl("file:///"));
}

PresentationView::~PresentationView() { delete m_bridge; }

void PresentationView::onBridgeImageGeom(int idx, double x, double y, double w, double h) {
    if (!m_deck || m_current < 0 || m_current >= m_deck->slides.size()) return;
    if (idx < 0 || idx >= m_deck->slides[m_current].images.size()) return;
    auto& img = m_deck->slides[m_current].images[idx];
    img.x = x; img.y = y; img.w = w; img.h = h;
    emit imageGeomChanged(idx, img);
}

void PresentationView::onBridgeImageSelected(int idx) {
    m_selected = idx;
    emit imageSelected(idx);
}

void PresentationView::onBridgeImageDropped(const QString& path, double x, double y) {
    if (!m_deck || m_current < 0 || m_current >= m_deck->slides.size()) return;
    SlideImage img;
    img.path = path;
    img.x = qBound(0.0, x - 0.2, 0.95);
    img.y = qBound(0.0, y - 0.2, 0.95);
    img.w = 0.4;
    img.h = 0.4;
    m_deck->slides[m_current].images.append(img);
    int newIdx = m_deck->slides[m_current].images.size() - 1;
    emit imageGeomChanged(newIdx, img);
    pushSlideToWeb(m_current);
}

void PresentationView::onBridgeContent(const QString& title, const QString& subtitle, const QString& bodyHtml) {
    if (!m_deck || m_current < 0 || m_current >= m_deck->slides.size()) return;
    auto& s = m_deck->slides[m_current];
    s.title = title;
    s.subtitle = subtitle;
    if (s.blocks.isEmpty()) s.blocks.append(TextBlock());
    // Konwertuj HTML na plain text (proste)
    QString plain = bodyHtml;
    plain.replace("<p>", "").replace("</p>", "\n");
    plain.replace("<br>", "\n").replace("<br/>", "\n");
    plain.replace("<h1>", "# ").replace("</h1>", "\n");
    plain.replace("<h2>", "## ").replace("</h2>", "\n");
    plain.replace("<h3>", "### ").replace("</h3>", "\n");
    plain.replace("<li>", "- ").replace("</li>", "\n");
    plain.remove(QRegularExpression("<[^>]+>"));
    s.blocks[0].text = plain.trimmed();
    emit contentChanged(title, subtitle, bodyHtml);
}

void PresentationView::renderSlide(int index) {
    if (!m_deck || index < 0 || index >= m_deck->slides.size()) return;
    int prevSel = m_selected;
    m_current = index;
    pushSlideToWeb(index);
    Q_UNUSED(prevSel);
}

void PresentationView::pushSlideToWeb(int index) {
    if (!m_deck || index < 0 || index >= m_deck->slides.size()) return;
    const Slide& s = m_deck->slides[index];

    QString bodyHtml;
    if (!s.blocks.isEmpty()) {
        QStringList lines = s.blocks.first().text.split('\n');
        QString curList;
        for (const QString& line : lines) {
            QString t = line.trimmed();
            if (t.isEmpty()) { if (!curList.isEmpty()) { bodyHtml += "</ul>"; curList.clear(); } bodyHtml += "<p></p>"; continue; }
            if (t.startsWith("### ")) bodyHtml += "<h3>" + t.mid(4).toHtmlEscaped() + "</h3>";
            else if (t.startsWith("## ")) bodyHtml += "<h2>" + t.mid(3).toHtmlEscaped() + "</h2>";
            else if (t.startsWith("# ")) bodyHtml += "<h1>" + t.mid(2).toHtmlEscaped() + "</h1>";
            else if (t.startsWith("- ")) {
                if (curList != "ul") { if (!curList.isEmpty()) bodyHtml += "</" + curList + ">"; bodyHtml += "<ul>"; curList = "ul"; }
                bodyHtml += "<li>" + t.mid(2).toHtmlEscaped() + "</li>";
            } else {
                if (!curList.isEmpty()) { bodyHtml += "</" + curList + ">"; curList.clear(); }
                bodyHtml += "<p>" + t.toHtmlEscaped() + "</p>";
            }
        }
        if (!curList.isEmpty()) bodyHtml += "</" + curList + ">";
    }

    QString bgColor = "#050308", fgColor = "#e0dde8";
    if (m_deck->theme == "light") { bgColor = "#ffffff"; fgColor = "#222222"; }
    else if (m_deck->theme == "dark") { bgColor = "#1a1a1a"; fgColor = "#e0e0e0"; }
    if (!s.backgroundColor.isEmpty()) bgColor = s.backgroundColor;

    QString bgImage;
    if (!s.backgroundImage.isEmpty()) {
        QString p = s.backgroundImage;
        if (!p.startsWith("http")) p = "file://" + QFileInfo(p).absoluteFilePath();
        bgImage = p;
    }

    QJsonArray imgsArr;
    for (const auto& img : s.images) {
        QJsonObject o;
        QString path = img.path;
        if (!path.startsWith("http")) path = QFileInfo(path).absoluteFilePath();
        o["path"] = path;
        o["x"] = img.x; o["y"] = img.y; o["w"] = img.w; o["h"] = img.h;
        o["srcX"] = img.srcX; o["srcY"] = img.srcY;
        o["srcW"] = img.srcW; o["srcH"] = img.srcH;
        o["zOrder"] = img.zOrder;
        o["preserveAspect"] = img.preserveAspect;
        imgsArr.append(o);
    }

    QJsonObject data;
    data["bgColor"] = bgColor;
    data["fgColor"] = fgColor;
    data["bgImage"] = bgImage;
    data["title"] = s.title;
    data["subtitle"] = s.subtitle;
    data["bodyHtml"] = bodyHtml;
    data["images"] = imgsArr;
    data["footerText"] = QString("%1 / %2").arg(index + 1).arg(m_deck->slides.size());

    QString json = QJsonDocument(data).toJson(QJsonDocument::Compact);
    m_web->page()->runJavaScript(QString("renderSlide(%1);").arg(json));
}

QPixmap PresentationView::renderThumbnail(const Slide& slide, int index, int total) const {
    QPixmap pm(320, 180);
    QColor bgColor(0x05, 0x03, 0x08);
    if (m_deck && m_deck->theme == "light") bgColor = Qt::white;
    else if (m_deck && m_deck->theme == "dark") bgColor = QColor(0x1a, 0x1a, 0x1a);
    if (!slide.backgroundColor.isEmpty()) bgColor = QColor(slide.backgroundColor);
    pm.fill(bgColor);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    if (!slide.backgroundImage.isEmpty()) {
        QImage img(slide.backgroundImage);
        if (!img.isNull())
            p.drawImage(pm.rect(), img.scaled(pm.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }

    for (const auto& si : slide.images) {
        QImage img(si.path);
        if (img.isNull()) continue;
        QRectF target(si.x * 320, si.y * 180, si.w * 320, si.h * 180);
        p.drawImage(target, img);
    }

    QColor fg = Qt::white;
    if (m_deck && m_deck->theme == "light") fg = Qt::black;
    QFont f("Arial", 12, QFont::Bold);
    p.setFont(f);
    p.setPen(fg);
    if (!slide.title.isEmpty())
        p.drawText(QRectF(10, 10, 300, 30), Qt::AlignLeft | Qt::AlignVCenter, slide.title);

    p.setFont(QFont("Arial", 8));
    p.setPen(QColor(255, 255, 255, 100));
    p.drawText(QRectF(0, 165, 310, 15), Qt::AlignRight, QString("%1 / %2").arg(index + 1).arg(total));

    return pm;
}

void PresentationView::cmdBold() { m_web->page()->runJavaScript("cmdBold();"); }
void PresentationView::cmdItalic() { m_web->page()->runJavaScript("cmdItalic();"); }
void PresentationView::cmdUnderline() { m_web->page()->runJavaScript("cmdUnderline();"); }
void PresentationView::cmdHeading1() { m_web->page()->runJavaScript("cmdH1();"); }
void PresentationView::cmdHeading2() { m_web->page()->runJavaScript("cmdH2();"); }
void PresentationView::cmdHeading3() { m_web->page()->runJavaScript("cmdH3();"); }
void PresentationView::cmdParagraph() { m_web->page()->runJavaScript("cmdP();"); }
void PresentationView::cmdBulletList() { m_web->page()->runJavaScript("cmdUL();"); }
void PresentationView::cmdNumberedList() { m_web->page()->runJavaScript("cmdOL();"); }
void PresentationView::cmdDeleteSelectedImage() { m_web->page()->runJavaScript("deleteSelected();"); }

#include "presentation_view.moc"
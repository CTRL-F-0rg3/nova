#include "main_window.hpp"
#include "title_bar.hpp"
#include "cosmic_background.hpp"
#include "toolbar.hpp"
#include "slide_editor.hpp"
#include "presentation_view.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QListWidget>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFileInfo>
#include <QKeyEvent>
#include <QPainter>
#include <QPdfWriter>
#include <QTextDocument>
#include <QApplication>
#include <QPixmap>
#include <QIcon>
#include <QRegularExpression>
#include <QDockWidget>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(1600, 950);
    setWindowTitle("Nova");

    setStyleSheet(
        "QMainWindow { background: #050308; }"
        "QWidget { background: #050308; color: #e0dde8; }"
        "QListWidget {"
        "  background: #0a0610; color: #d0c8e0; border: none;"
        "  border-right: 1px solid #2a2438; padding: 8px; font-size: 12px;"
        "}"
        "QListWidget::item {"
        "  background: #12081a; color: #d0c8e0; padding: 4px;"
        "  border-radius: 6px; margin: 4px; border: 1px solid #2a2438;"
        "}"
        "QListWidget::item:hover { border-color: #ff2d95; background: #1a0d22; }"
        "QListWidget::item:selected { background: rgba(255,45,149,0.25); border: 2px solid #ff2d95; color: #ff2d95; }"
        "QLabel { color: #a890c4; background: transparent; }"
        "QLineEdit, QTextEdit, QComboBox, QDoubleSpinBox, QSpinBox {"
        "  background: #0a0610; color: #e0dde8; border: 1px solid #2a2438;"
        "  border-radius: 4px; padding: 6px;"
        "}"
        "QLineEdit:focus, QTextEdit:focus { border: 1px solid #ff2d95; }"
        "QPushButton {"
        "  background: #1a0d22; color: #e0dde8; border: 1px solid #2a2438;"
        "  border-radius: 4px; padding: 6px 12px; font-weight: bold;"
        "}"
        "QPushButton:hover { border-color: #ff2d95; color: #ff2d95; }"
        "QGroupBox {"
        "  color: #a890c4; border: 1px solid #2a2438; border-radius: 4px;"
        "  margin-top: 8px; padding-top: 8px; background: transparent;"
        "}"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; background: #050308; }"
        "QDockWidget { color: #e0dde8; }"
        "QDockWidget::title { background: #150a14; padding: 6px; border-bottom: 1px solid #ff2d95; }"
        "QSplitter { background: #050308; }"
        "QSplitter::handle { background: #2a2438; width: 2px; }"
        "QSplitter::handle:hover { background: #ff2d95; }"
        "QScrollBar:vertical { background: #050308; width: 12px; }"
        "QScrollBar::handle:vertical { background: #2a2438; border-radius: 6px; min-height: 30px; }"
        "QScrollBar::handle:vertical:hover { background: #ff2d95; }"
        "QScrollBar:horizontal { background: #050308; height: 12px; }"
        "QScrollBar::handle:horizontal { background: #2a2438; border-radius: 6px; min-width: 30px; }"
        "QScrollBar::handle:horizontal:hover { background: #ff2d95; }"
        "QScrollBar::add-line, QScrollBar::sub-line { height: 0; width: 0; }"
        "QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }"
        "QMenuBar { background: transparent; color: #e0dde8; }"
        "QMenuBar::item { background: transparent; padding: 6px 12px; }"
        "QMenuBar::item:selected { background: rgba(255,45,149,0.2); color: #ff2d95; }"
        "QMenu { background: #150a14; color: #e0dde8; border: 1px solid #ff2d95; border-radius: 4px; }"
        "QMenu::item { padding: 6px 24px; }"
        "QMenu::item:selected { background: #ff2d95; color: white; }"
    );

    auto* central = new CosmicBackground(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_titleBar = new TitleBar(central);
    layout->addWidget(m_titleBar);

    auto* fileMenu = m_titleBar->menuBar()->addMenu("File");
    fileMenu->addAction("New", QKeySequence::New, this, &MainWindow::newDeck);
    fileMenu->addAction("Open", QKeySequence::Open, this, &MainWindow::openDeck);
    fileMenu->addAction("Save", QKeySequence::Save, this, &MainWindow::saveDeck);
    fileMenu->addSeparator();
    fileMenu->addAction("Export PDF", QKeySequence("Ctrl+E"), this, &MainWindow::exportPdf);

    m_toolbar = new Toolbar(central);
    layout->addWidget(m_toolbar);

    m_mainSplitter = new QSplitter(Qt::Horizontal, central);
    m_mainSplitter->setStyleSheet(
        "QSplitter { background: #050308; }"
        "QSplitter::handle { background: #2a2438; width: 2px; }"
        "QSplitter::handle:hover { background: #ff2d95; }"
    );

    m_slideList = new QListWidget(m_mainSplitter);
    m_slideList->setIconSize(QSize(180, 101));
    m_slideList->setMinimumWidth(220);
    m_mainSplitter->addWidget(m_slideList);

    m_canvas = new PresentationView(m_mainSplitter);
    m_mainSplitter->addWidget(m_canvas);

    m_mainSplitter->setStretchFactor(0, 0);
    m_mainSplitter->setStretchFactor(1, 1);
    m_mainSplitter->setSizes({220, 1380});

    layout->addWidget(m_mainSplitter, 1);
    setCentralWidget(central);

    m_properties = new SlideEditor(this);
    m_propertiesDock = new QDockWidget("Właściwości", this);
    m_propertiesDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_propertiesDock->setWidget(m_properties);
    m_propertiesDock->setMinimumWidth(320);
    addDockWidget(Qt::RightDockWidgetArea, m_propertiesDock);
    m_propertiesDock->hide();

    connect(m_slideList, &QListWidget::currentRowChanged, this, &MainWindow::onSlideSelected);
    connect(m_canvas, &PresentationView::slideNavigationRequested, this, &MainWindow::navigateSlide);
    connect(m_canvas, &PresentationView::imageGeomChanged, this, &MainWindow::onPreviewImageChanged);
    connect(m_canvas, &PresentationView::imageSelected, this, &MainWindow::onPreviewImageSelected);

    connect(m_toolbar, &Toolbar::newDeckRequested, this, &MainWindow::newDeck);
    connect(m_toolbar, &Toolbar::openRequested, this, &MainWindow::openDeck);
    connect(m_toolbar, &Toolbar::saveRequested, this, &MainWindow::saveDeck);
    connect(m_toolbar, &Toolbar::addSlideRequested, this, &MainWindow::addSlide);
    connect(m_toolbar, &Toolbar::duplicateSlideRequested, this, &MainWindow::duplicateSlide);
    connect(m_toolbar, &Toolbar::removeSlideRequested, this, &MainWindow::removeSlide);
    connect(m_toolbar, &Toolbar::themeChanged, this, &MainWindow::onThemeChanged);
    connect(m_toolbar, &Toolbar::presentRequested, this, &MainWindow::present);
    connect(m_toolbar, &Toolbar::exportPdfRequested, this, &MainWindow::exportPdf);
    connect(m_toolbar, &Toolbar::propertiesToggled, this, &MainWindow::toggleProperties);

    connect(m_toolbar, &Toolbar::boldRequested, m_canvas, &PresentationView::cmdBold);
    connect(m_toolbar, &Toolbar::italicRequested, m_canvas, &PresentationView::cmdItalic);
    connect(m_toolbar, &Toolbar::underlineRequested, m_canvas, &PresentationView::cmdUnderline);
    connect(m_toolbar, &Toolbar::h1Requested, m_canvas, &PresentationView::cmdHeading1);
    connect(m_toolbar, &Toolbar::h2Requested, m_canvas, &PresentationView::cmdHeading2);
    connect(m_toolbar, &Toolbar::h3Requested, m_canvas, &PresentationView::cmdHeading3);
    connect(m_toolbar, &Toolbar::pRequested, m_canvas, &PresentationView::cmdParagraph);
    connect(m_toolbar, &Toolbar::ulRequested, m_canvas, &PresentationView::cmdBulletList);
    connect(m_toolbar, &Toolbar::olRequested, m_canvas, &PresentationView::cmdNumberedList);

    connect(m_properties, &SlideEditor::slideChanged, this, [this]{
        if (m_currentSlide >= 0 && m_currentSlide < m_deck.slides.size()) {
            m_deck.slides[m_currentSlide] = m_properties->slide();
            m_canvas->renderSlide(m_currentSlide);
            refreshSlideList();
        }
    });
    connect(m_properties, &SlideEditor::imageSelected, this, [this](int idx){
        Q_UNUSED(idx);
    });

    m_canvas->setDeck(&m_deck);
    refreshSlideList();
    onSlideSelected(0);
}

void MainWindow::toggleProperties() {
    m_propertiesDock->setVisible(!m_propertiesDock->isVisible());
}

void MainWindow::refreshSlideList() {
    m_updatingList = true;
    int currentRow = m_slideList->currentRow();
    m_slideList->clear();

    for (int i = 0; i < m_deck.slides.size(); ++i) {
        QPixmap thumb = m_canvas->renderThumbnail(m_deck.slides[i], i, m_deck.slides.size());
        QString label = QString::number(i + 1);
        auto* item = new QListWidgetItem(QIcon(thumb), label);
        item->setSizeHint(QSize(200, 130));
        m_slideList->addItem(item);
    }

    if (currentRow >= 0 && currentRow < m_slideList->count())
        m_slideList->setCurrentRow(currentRow);
    else if (m_currentSlide < m_slideList->count())
        m_slideList->setCurrentRow(m_currentSlide);

    m_updatingList = false;
}

void MainWindow::renderCurrentSlide() {
    if (m_currentSlide < 0 || m_currentSlide >= m_deck.slides.size()) return;
    m_canvas->renderSlide(m_currentSlide);
    m_properties->setSlide(m_deck.slides[m_currentSlide]);
}

void MainWindow::onSlideSelected(int idx) {
    if (m_updatingList || idx < 0 || idx >= m_deck.slides.size()) return;
    m_currentSlide = idx;
    renderCurrentSlide();
}

void MainWindow::newDeck() {
    m_deck = SlideDeck();
    m_currentSlide = 0;
    m_currentPath.clear();
    m_canvas->setDeck(&m_deck);
    refreshSlideList();
    renderCurrentSlide();
    setWindowTitle("Nova");
}

void MainWindow::openDeck() {
    QString path = QFileDialog::getOpenFileName(this, "Otwórz prezentację",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Nova (*.nova);;JSON (*.json)");
    if (path.isEmpty()) return;
    m_deck = SlideDeck::load(path);
    m_currentPath = path;
    m_currentSlide = 0;
    m_canvas->setDeck(&m_deck);
    refreshSlideList();
    renderCurrentSlide();
    setWindowTitle("Nova — " + QFileInfo(path).fileName());
}

void MainWindow::saveDeck() {
    QString path = m_currentPath;
    if (path.isEmpty()) {
        path = QFileDialog::getSaveFileName(this, "Zapisz prezentację",
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/prezentacja.nova",
            "Nova (*.nova)");
        if (path.isEmpty()) return;
        if (!path.endsWith(".nova")) path += ".nova";
    }
    if (!m_deck.save(path)) {
        QMessageBox::warning(this, "Błąd", "Nie można zapisać pliku.");
        return;
    }
    m_currentPath = path;
    setWindowTitle("Nova — " + QFileInfo(path).fileName());
}

void MainWindow::addSlide() {
    m_deck.addSlide(m_currentSlide + 1);
    m_currentSlide++;
    refreshSlideList();
    renderCurrentSlide();
}

void MainWindow::duplicateSlide() {
    m_deck.duplicateSlide(m_currentSlide);
    m_currentSlide++;
    refreshSlideList();
    renderCurrentSlide();
}

void MainWindow::removeSlide() {
    if (m_deck.slides.size() <= 1) return;
    m_deck.removeSlide(m_currentSlide);
    if (m_currentSlide >= m_deck.slides.size()) m_currentSlide = m_deck.slides.size() - 1;
    refreshSlideList();
    renderCurrentSlide();
}

void MainWindow::onThemeChanged(const QString& theme) {
    m_deck.theme = theme;
    refreshSlideList();
    m_canvas->renderSlide(m_currentSlide);
}

void MainWindow::navigateSlide(int delta) {
    int newIdx = m_currentSlide + delta;
    if (newIdx < 0 || newIdx >= m_deck.slides.size()) return;
    m_currentSlide = newIdx;
    m_slideList->setCurrentRow(m_currentSlide);
    renderCurrentSlide();
}

void MainWindow::onPreviewImageChanged(int imgIdx, const SlideImage& img) {
    if (m_currentSlide < 0 || m_currentSlide >= m_deck.slides.size()) return;
    if (imgIdx < 0 || imgIdx >= m_deck.slides[m_currentSlide].images.size()) return;
    m_deck.slides[m_currentSlide].images[imgIdx] = img;
    if (m_propertiesDock->isVisible())
        m_properties->refreshFromDeckImage(imgIdx, img);
}

void MainWindow::onPreviewImageSelected(int imgIdx) {
    if (m_propertiesDock->isVisible())
        m_properties->selectImage(imgIdx);
}

void MainWindow::present() {
    auto* w = new QWidget(nullptr, Qt::Window | Qt::FramelessWindowHint);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setStyleSheet("background: black;");
    w->resize(1920, 1080);

    auto* layout = new QVBoxLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);
    auto* pv = new PresentationView(w);
    pv->setDeck(&m_deck);
    layout->addWidget(pv);

    pv->renderSlide(m_currentSlide);
    int* pos = new int(m_currentSlide);
    connect(pv, &PresentationView::slideNavigationRequested, w, [pv, pos](int delta) {
        *pos += delta;
        if (*pos < 0) *pos = 0;
        if (pv->currentSlide() >= 0) pv->renderSlide(*pos);
    });
    connect(w, &QWidget::destroyed, w, [pos]{ delete pos; });

    w->showFullScreen();
}

void MainWindow::exportPdf() {
    QString path = QFileDialog::getSaveFileName(this, "Eksport do PDF",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/prezentacja.pdf",
        "PDF (*.pdf)");
    if (path.isEmpty()) return;

    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageOrientation(QPageLayout::Landscape);
    writer.setResolution(300);

    QPainter painter(&writer);
    QRect r(0, 0, writer.width(), writer.height());

    for (int i = 0; i < m_deck.slides.size(); ++i) {
        if (i > 0) writer.newPage();
        const Slide& s = m_deck.slides[i];

        QColor bgColor(0x05, 0x03, 0x08);
        if (m_deck.theme == "light") bgColor = Qt::white;
        else if (m_deck.theme == "dark") bgColor = QColor(0x1a, 0x1a, 0x1a);
        if (!s.backgroundColor.isEmpty()) bgColor = QColor(s.backgroundColor);
        painter.fillRect(r, bgColor);

        for (const auto& si : s.images) {
            QImage img(si.path);
            if (img.isNull()) continue;
            QRectF target(si.x * r.width(), si.y * r.height(), si.w * r.width(), si.h * r.height());
            painter.drawImage(target, img);
        }

        painter.setPen(QColor(0xff, 0x2d, 0x95));
        painter.setFont(QFont("Helvetica", 72, QFont::Bold));
        painter.drawText(QRectF(r.width() * 0.08, r.height() * 0.08,
                                r.width() * 0.84, r.height() * 0.2),
                         Qt::AlignLeft, s.title);
    }
    painter.end();
    QMessageBox::information(this, "Eksport", "Zapisano: " + path);
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
    if (e->matches(QKeySequence::Save)) { saveDeck(); return; }
    if (e->matches(QKeySequence::New)) { newDeck(); return; }
    if (e->matches(QKeySequence::Open)) { openDeck(); return; }
    if (e->key() == Qt::Key_F5) { present(); return; }
    QMainWindow::keyPressEvent(e);
}
#include "slide_editor.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QFileInfo>
#include <QScrollArea>

SlideEditor::SlideEditor(QWidget* parent) : QWidget(parent) {
    setStyleSheet(
        "QWidget { background: #0a0610; color: #e0dde8; }"
        "QLabel { color: #a890c4; font-size: 11px; font-weight: bold; background: transparent; }"
        "QLineEdit, QComboBox, QDoubleSpinBox {"
        "  background: #050308; color: #e0dde8; border: 1px solid #2a2438;"
        "  border-radius: 4px; padding: 6px;"
        "}"
        "QLineEdit:focus { border: 1px solid #ff2d95; }"
        "QPushButton {"
        "  background: #1a0d22; color: #e0dde8; border: 1px solid #2a2438;"
        "  border-radius: 4px; padding: 6px 12px; font-weight: bold;"
        "}"
        "QPushButton:hover { border-color: #ff2d95; color: #ff2d95; }"
        "QPushButton#addImg { background: #00ff88; color: #000; border-color: #00ff88; }"
        "QPushButton#rmImg { background: rgba(255,60,60,0.8); color: white; border-color: transparent; }"
        "QListWidget { background: #050308; color: #e0dde8; border: 1px solid #2a2438; border-radius: 4px; }"
        "QListWidget::item { padding: 6px; border-bottom: 1px solid #1a0d22; }"
        "QListWidget::item:selected { background: rgba(255,45,149,0.3); color: #ff2d95; }"
        "QGroupBox {"
        "  color: #a890c4; border: 1px solid #2a2438; border-radius: 4px;"
        "  margin-top: 8px; padding-top: 8px; background: transparent;"
        "}"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; background: #0a0610; }"
    );

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    outer->addWidget(scroll);

    auto* content = new QWidget();
    content->setStyleSheet("background: transparent;");
    scroll->setWidget(content);

    auto* layout = new QVBoxLayout(content);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);

    auto addLabel = [&](const QString& text) {
        auto* l = new QLabel(text, content);
        layout->addWidget(l);
    };

    // Tło
    addLabel("Kolor tła (#hex):");
    auto* bgRow = new QHBoxLayout();
    m_bgColorEdit = new QLineEdit(content);
    m_bgColorEdit->setPlaceholderText("#150a14");
    bgRow->addWidget(m_bgColorEdit);
    auto* bgPick = new QPushButton("Wybierz", content);
    bgRow->addWidget(bgPick);
    layout->addLayout(bgRow);

    // Obrazy
    addLabel("Obrazy na slajdzie:");
    m_imageList = new QListWidget(content);
    m_imageList->setMaximumHeight(120);
    layout->addWidget(m_imageList);

    auto* imgBtnRow = new QHBoxLayout();
    m_addImageBtn = new QPushButton("+ Dodaj obraz", content);
    m_addImageBtn->setObjectName("addImg");
    m_removeImageBtn = new QPushButton("- Usuń", content);
    m_removeImageBtn->setObjectName("rmImg");
    imgBtnRow->addWidget(m_addImageBtn);
    imgBtnRow->addWidget(m_removeImageBtn);
    layout->addLayout(imgBtnRow);

    // Pozycja i rozmiar
    m_imgControls = new QGroupBox("Pozycja i rozmiar", content);
    auto* imgForm = new QFormLayout(m_imgControls);
    m_imgX = new QDoubleSpinBox(content); m_imgX->setRange(0, 1); m_imgX->setSingleStep(0.01); m_imgX->setDecimals(3);
    m_imgY = new QDoubleSpinBox(content); m_imgY->setRange(0, 1); m_imgY->setSingleStep(0.01); m_imgY->setDecimals(3);
    m_imgW = new QDoubleSpinBox(content); m_imgW->setRange(0.05, 1); m_imgW->setSingleStep(0.01); m_imgW->setDecimals(3);
    m_imgH = new QDoubleSpinBox(content); m_imgH->setRange(0.05, 1); m_imgH->setSingleStep(0.01); m_imgH->setDecimals(3);
    imgForm->addRow("X:", m_imgX);
    imgForm->addRow("Y:", m_imgY);
    imgForm->addRow("Szer:", m_imgW);
    imgForm->addRow("Wys:", m_imgH);
    layout->addWidget(m_imgControls);

    // Kadrowanie
    m_cropControls = new QGroupBox("Kadrowanie", content);
    auto* cropForm = new QFormLayout(m_cropControls);
    m_cropX = new QDoubleSpinBox(content); m_cropX->setRange(0, 1); m_cropX->setSingleStep(0.01); m_cropX->setDecimals(3);
    m_cropY = new QDoubleSpinBox(content); m_cropY->setRange(0, 1); m_cropY->setSingleStep(0.01); m_cropY->setDecimals(3);
    m_cropW = new QDoubleSpinBox(content); m_cropW->setRange(0.01, 1); m_cropW->setSingleStep(0.01); m_cropW->setDecimals(3);
    m_cropH = new QDoubleSpinBox(content); m_cropH->setRange(0.01, 1); m_cropH->setSingleStep(0.01); m_cropH->setDecimals(3);
    cropForm->addRow("Crop X:", m_cropX);
    cropForm->addRow("Crop Y:", m_cropY);
    cropForm->addRow("Crop Szer:", m_cropW);
    cropForm->addRow("Crop Wys:", m_cropH);
    layout->addWidget(m_cropControls);

    layout->addStretch();

    connect(m_bgColorEdit, &QLineEdit::textChanged, this, &SlideEditor::onBgColorChanged);
    connect(bgPick, &QPushButton::clicked, this, [this]{
        QColor c = QColorDialog::getColor(Qt::black, this, "Kolor tła");
        if (c.isValid()) m_bgColorEdit->setText(c.name());
    });
    connect(m_addImageBtn, &QPushButton::clicked, this, &SlideEditor::onImageAdd);
    connect(m_removeImageBtn, &QPushButton::clicked, this, &SlideEditor::onImageRemove);
    connect(m_imageList, &QListWidget::currentRowChanged, this, &SlideEditor::onImageSelected);
    connect(m_imgX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SlideEditor::onImageGeometryChanged);
    connect(m_imgY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SlideEditor::onImageGeometryChanged);
    connect(m_imgW, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SlideEditor::onImageGeometryChanged);
    connect(m_imgH, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SlideEditor::onImageGeometryChanged);
    connect(m_cropX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SlideEditor::onCropChanged);
    connect(m_cropY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SlideEditor::onCropChanged);
    connect(m_cropW, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SlideEditor::onCropChanged);
    connect(m_cropH, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SlideEditor::onCropChanged);

    updateImageEditorState();
}

void SlideEditor::setSlide(const Slide& s) {
    m_updating = true;
    m_slide = s;
    m_bgColorEdit->setText(s.backgroundColor);
    m_selectedImage = -1;
    refreshImageList();
    updateImageEditorState();
    m_updating = false;
}

void SlideEditor::refreshImageList() {
    m_imageList->clear();
    for (int i = 0; i < m_slide.images.size(); ++i) {
        QString path = m_slide.images[i].path;
        QString label = QFileInfo(path).fileName();
        if (label.isEmpty()) label = QString("Obraz %1").arg(i + 1);
        if (label.length() > 30) label = label.left(27) + "...";
        m_imageList->addItem(label);
    }
    if (m_selectedImage >= 0 && m_selectedImage < m_imageList->count())
        m_imageList->setCurrentRow(m_selectedImage);
}

void SlideEditor::updateImageEditorState() {
    bool has = m_selectedImage >= 0 && m_selectedImage < m_slide.images.size();
    m_imgControls->setEnabled(has);
    m_cropControls->setEnabled(has);
    m_removeImageBtn->setEnabled(has);
    if (has) {
        auto& img = m_slide.images[m_selectedImage];
        m_imgX->blockSignals(true); m_imgX->setValue(img.x); m_imgX->blockSignals(false);
        m_imgY->blockSignals(true); m_imgY->setValue(img.y); m_imgY->blockSignals(false);
        m_imgW->blockSignals(true); m_imgW->setValue(img.w); m_imgW->blockSignals(false);
        m_imgH->blockSignals(true); m_imgH->setValue(img.h); m_imgH->blockSignals(false);
        m_cropX->blockSignals(true); m_cropX->setValue(img.srcX); m_cropX->blockSignals(false);
        m_cropY->blockSignals(true); m_cropY->setValue(img.srcY); m_cropY->blockSignals(false);
        m_cropW->blockSignals(true); m_cropW->setValue(img.srcW); m_cropW->blockSignals(false);
        m_cropH->blockSignals(true); m_cropH->setValue(img.srcH); m_cropH->blockSignals(false);
    }
}

void SlideEditor::selectImage(int idx) {
    m_selectedImage = idx;
    if (idx >= 0 && idx < m_imageList->count()) {
        m_imageList->blockSignals(true);
        m_imageList->setCurrentRow(idx);
        m_imageList->blockSignals(false);
    }
    updateImageEditorState();
}

void SlideEditor::refreshFromDeckImage(int idx, const SlideImage& img) {
    if (idx < 0 || idx >= m_slide.images.size()) return;
    m_slide.images[idx] = img;
    if (idx == m_selectedImage) updateImageEditorState();
}

void SlideEditor::onBgColorChanged() {
    if (m_updating) return;
    m_slide.backgroundColor = m_bgColorEdit->text();
    m_slide.backgroundImage.clear();
    emit slideChanged();
}

void SlideEditor::onImageAdd() {
    QString path = QFileDialog::getOpenFileName(this, "Wybierz obraz", QString(),
        "Obrazy (*.png *.jpg *.jpeg *.gif *.bmp *.webp)");
    if (path.isEmpty()) return;
    SlideImage img;
    img.path = path;
    img.x = 0.1; img.y = 0.1; img.w = 0.4; img.h = 0.4;
    m_slide.images.append(img);
    m_selectedImage = m_slide.images.size() - 1;
    refreshImageList();
    updateImageEditorState();
    emit imageSelected(m_selectedImage);
    emit slideChanged();
}

void SlideEditor::onImageRemove() {
    if (m_selectedImage < 0 || m_selectedImage >= m_slide.images.size()) return;
    m_slide.images.removeAt(m_selectedImage);
    m_selectedImage = -1;
    refreshImageList();
    updateImageEditorState();
    emit imageSelected(-1);
    emit slideChanged();
}

void SlideEditor::onImageSelected(int row) {
    m_selectedImage = row;
    updateImageEditorState();
    emit imageSelected(row);
}

void SlideEditor::onImageGeometryChanged() {
    if (m_updating) return;
    if (m_selectedImage < 0 || m_selectedImage >= m_slide.images.size()) return;
    auto& img = m_slide.images[m_selectedImage];
    img.x = m_imgX->value(); img.y = m_imgY->value();
    img.w = m_imgW->value(); img.h = m_imgH->value();
    emit slideChanged();
}

void SlideEditor::onCropChanged() {
    if (m_updating) return;
    if (m_selectedImage < 0 || m_selectedImage >= m_slide.images.size()) return;
    auto& img = m_slide.images[m_selectedImage];
    img.srcX = m_cropX->value(); img.srcY = m_cropY->value();
    img.srcW = m_cropW->value(); img.srcH = m_cropH->value();
    emit slideChanged();
}
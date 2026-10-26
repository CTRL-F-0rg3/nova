#include "toolbar.hpp"
#include <QHBoxLayout>
#include <QToolButton>
#include <QComboBox>
#include <QFrame>

Toolbar::Toolbar(QWidget* parent) : QWidget(parent) {
    setObjectName("novaToolbar");
    setStyleSheet(
        "QWidget#novaToolbar {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "    stop:0 rgba(30, 18, 40, 240), stop:1 rgba(15, 8, 22, 240));"
        "  border-bottom: 1px solid rgba(255, 45, 149, 80);"
        "}"
    );
    setFixedHeight(90);

    auto* l = new QHBoxLayout(this);
    l->setContentsMargins(12, 6, 12, 6);
    l->setSpacing(4);

    auto makeBar = [&](const QString& text, const QString& tip) {
        auto* b = new QToolButton(this);
        b->setText(text);
        b->setToolTip(tip);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(
            "QToolButton { background: rgba(0,0,0,0.5); color: #e0dde8; "
            "border: 1px solid #2a2438; border-radius: 4px; padding: 4px 10px; font-weight: bold; min-width: 32px; }"
            "QToolButton:hover { border-color: #ff2d95; color: #ff2d95; }"
        );
        l->addWidget(b);
        return b;
    };

    // File
    auto* newBtn = makeBar("New", "Nowa prezentacja");
    auto* openBtn = makeBar("Open", "Otwórz");
    auto* saveBtn = makeBar("Save", "Zapisz");
    saveBtn->setStyleSheet("QToolButton { background: #ff2d95; color: white; border: 1px solid #ff2d95; border-radius: 4px; padding: 4px 12px; font-weight: bold; min-width: 32px; }");

    auto* v1 = new QFrame(this); v1->setFrameShape(QFrame::VLine); l->addWidget(v1);

    // Format
    auto* boldBtn = makeBar("B", "Pogrubienie (Ctrl+B)");
    boldBtn->setStyleSheet(boldBtn->styleSheet() + " QToolButton { font-weight: 900; }");
    auto* italicBtn = makeBar("I", "Kursywa (Ctrl+I)");
    auto* underlineBtn = makeBar("U", "Podkreślenie");
    auto* h1Btn = makeBar("H1", "Nagłówek 1");
    auto* h2Btn = makeBar("H2", "Nagłówek 2");
    auto* h3Btn = makeBar("H3", "Nagłówek 3");
    auto* pBtn = makeBar("P", "Akapit");
    auto* ulBtn = makeBar("•", "Lista punktowana");
    auto* olBtn = makeBar("1.", "Lista numerowana");

    auto* v2 = new QFrame(this); v2->setFrameShape(QFrame::VLine); l->addWidget(v2);

    // Slide
    auto* addBtn = makeBar("+ Slajd", "Nowy slajd");
    auto* dupBtn = makeBar("Duplikuj", "Duplikuj slajd");
    auto* rmBtn = makeBar("- Slajd", "Usuń slajd");

    auto* themeCombo = new QComboBox(this);
    themeCombo->addItem("Cosmic", "cosmic");
    themeCombo->addItem("Dark", "dark");
    themeCombo->addItem("Light", "light");
    themeCombo->setStyleSheet(
        "QComboBox { background: rgba(0,0,0,0.5); color: #e0dde8; "
        "border: 1px solid #2a2438; border-radius: 4px; padding: 4px 8px; }"
    );
    l->addWidget(themeCombo);

    l->addStretch();

    auto* propsBtn = makeBar("Właściwości", "Panel właściwości");
    auto* presentBtn = makeBar("Prezentuj", "Tryb pełnoekranowy");
    presentBtn->setStyleSheet("QToolButton { background: #00ff88; color: #000; border: 1px solid #00ff88; border-radius: 4px; padding: 4px 14px; font-weight: bold; }");
    auto* exportBtn = makeBar("Export PDF", "Eksportuj do PDF");

    connect(newBtn, &QToolButton::clicked, this, &Toolbar::newDeckRequested);
    connect(openBtn, &QToolButton::clicked, this, &Toolbar::openRequested);
    connect(saveBtn, &QToolButton::clicked, this, &Toolbar::saveRequested);
    connect(addBtn, &QToolButton::clicked, this, &Toolbar::addSlideRequested);
    connect(dupBtn, &QToolButton::clicked, this, &Toolbar::duplicateSlideRequested);
    connect(rmBtn, &QToolButton::clicked, this, &Toolbar::removeSlideRequested);
    connect(propsBtn, &QToolButton::clicked, this, &Toolbar::propertiesToggled);
    connect(presentBtn, &QToolButton::clicked, this, &Toolbar::presentRequested);
    connect(exportBtn, &QToolButton::clicked, this, &Toolbar::exportPdfRequested);
    connect(themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, themeCombo](int){
        emit themeChanged(themeCombo->currentData().toString());
    });

    connect(boldBtn, &QToolButton::clicked, this, &Toolbar::boldRequested);
    connect(italicBtn, &QToolButton::clicked, this, &Toolbar::italicRequested);
    connect(underlineBtn, &QToolButton::clicked, this, &Toolbar::underlineRequested);
    connect(h1Btn, &QToolButton::clicked, this, &Toolbar::h1Requested);
    connect(h2Btn, &QToolButton::clicked, this, &Toolbar::h2Requested);
    connect(h3Btn, &QToolButton::clicked, this, &Toolbar::h3Requested);
    connect(pBtn, &QToolButton::clicked, this, &Toolbar::pRequested);
    connect(ulBtn, &QToolButton::clicked, this, &Toolbar::ulRequested);
    connect(olBtn, &QToolButton::clicked, this, &Toolbar::olRequested);
}

QToolButton* Toolbar::makeBtn(const QString& text, const QString& tip) {
    auto* b = new QToolButton(this);
    b->setText(text);
    b->setToolTip(tip);
    return b;
}
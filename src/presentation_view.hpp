#pragma once
#include <QWidget>
#include "slide_deck.hpp"

class DropWebView;
class BridgeObject;

class PresentationView : public QWidget {
    Q_OBJECT
public:
    explicit PresentationView(QWidget* parent = nullptr);
    ~PresentationView();

    void setDeck(SlideDeck* deck) { m_deck = deck; }
    void renderSlide(int index);
    int currentSlide() const { return m_current; }

    QPixmap renderThumbnail(const Slide& slide, int index, int total) const;

    // Format commands (wywoływane z toolbar)
    void cmdBold();
    void cmdItalic();
    void cmdUnderline();
    void cmdHeading1();
    void cmdHeading2();
    void cmdHeading3();
    void cmdParagraph();
    void cmdBulletList();
    void cmdNumberedList();
    void cmdDeleteSelectedImage();

signals:
    void slideNavigationRequested(int delta);
    void imageGeomChanged(int imgIdx, const SlideImage& img);
    void imageSelected(int imgIdx);
    void contentChanged(const QString& title, const QString& subtitle, const QString& bodyHtml);

private slots:
    void onBridgeImageGeom(int idx, double x, double y, double w, double h);
    void onBridgeImageSelected(int idx);
    void onBridgeImageDropped(const QString& path, double x, double y);
    void onBridgeContent(const QString& title, const QString& subtitle, const QString& bodyHtml);

private:
    void pushSlideToWeb(int index);

    SlideDeck* m_deck = nullptr;
    DropWebView* m_web;
    BridgeObject* m_bridge;
    int m_current = 0;
    int m_selected = -1;
};
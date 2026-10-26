#pragma once
#include <QMainWindow>
#include "slide_deck.hpp"

class QListWidget;
class TitleBar;
class Toolbar;
class SlideEditor;
class PresentationView;
class QSplitter;
class QDockWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* e) override;

private slots:
    void newDeck();
    void openDeck();
    void saveDeck();
    void addSlide();
    void duplicateSlide();
    void removeSlide();
    void onSlideSelected(int idx);
    void onThemeChanged(const QString& theme);
    void present();
    void exportPdf();
    void navigateSlide(int delta);
    void onPreviewImageChanged(int imgIdx, const SlideImage& img);
    void onPreviewImageSelected(int imgIdx);
    void toggleProperties();

private:
    void refreshSlideList();
    void renderCurrentSlide();

    TitleBar* m_titleBar;
    Toolbar* m_toolbar;
    QListWidget* m_slideList;
    PresentationView* m_canvas;
    SlideEditor* m_properties;
    QDockWidget* m_propertiesDock;
    QSplitter* m_mainSplitter;
    SlideDeck m_deck;
    int m_currentSlide = 0;
    QString m_currentPath;
    bool m_updatingList = false;
};
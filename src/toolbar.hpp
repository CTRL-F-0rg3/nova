#pragma once
#include <QWidget>

class QToolButton;
class QComboBox;

class Toolbar : public QWidget {
    Q_OBJECT
public:
    explicit Toolbar(QWidget* parent = nullptr);

signals:
    void newDeckRequested();
    void openRequested();
    void saveRequested();
    void addSlideRequested();
    void removeSlideRequested();
    void duplicateSlideRequested();
    void themeChanged(const QString& theme);
    void presentRequested();
    void exportPdfRequested();
    void propertiesToggled();

    void boldRequested();
    void italicRequested();
    void underlineRequested();
    void h1Requested();
    void h2Requested();
    void h3Requested();
    void pRequested();
    void ulRequested();
    void olRequested();

private:
    QToolButton* makeBtn(const QString& text, const QString& tip);
};
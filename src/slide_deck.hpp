#pragma once
#include "slide.hpp"
#include <QString>

class SlideDeck {
public:
    SlideDeck();

    QString name = "Untitled";
    QList<Slide> slides;
    QString theme = "cosmic";  // cosmic, dark, light

    void addSlide(int index = -1);
    void removeSlide(int index);
    void duplicateSlide(int index);

    bool save(const QString& path) const;
    static SlideDeck load(const QString& path);

    static QString themeStylesheet(const QString& theme);
};
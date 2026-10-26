#pragma once
#include <QString>
#include <QList>
#include <QColor>
#include <QJsonObject>
#include <QJsonArray>

struct TextBlock {
    QString text;
    QString type = "p";
    bool bold = false;
    bool italic = false;
    QString color;
    int fontSize = 0;
};

struct SlideImage {
    QString path;
    qreal x = 0.1;
    qreal y = 0.1;
    qreal w = 0.8;
    qreal h = 0.6;
    qreal srcX = 0.0;
    qreal srcY = 0.0;
    qreal srcW = 1.0;
    qreal srcH = 1.0;
    int zOrder = 0;
    bool preserveAspect = true;

    QJsonObject toJson() const;
    static SlideImage fromJson(const QJsonObject& o);
};

struct Slide {
    QString title;
    QString subtitle;
    QList<TextBlock> blocks;
    QList<SlideImage> images;
    QString backgroundImage;
    QString backgroundColor;
    QString layout = "title-content";

    QJsonObject toJson() const;
    static Slide fromJson(const QJsonObject& o);
};
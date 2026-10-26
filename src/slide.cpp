#include "slide.hpp"

QJsonObject SlideImage::toJson() const {
    QJsonObject o;
    o["path"] = path;
    o["x"] = x;
    o["y"] = y;
    o["w"] = w;
    o["h"] = h;
    o["srcX"] = srcX;
    o["srcY"] = srcY;
    o["srcW"] = srcW;
    o["srcH"] = srcH;
    o["zOrder"] = zOrder;
    o["preserveAspect"] = preserveAspect;
    return o;
}

SlideImage SlideImage::fromJson(const QJsonObject& o) {
    SlideImage img;
    img.path = o["path"].toString();
    img.x = o["x"].toDouble(0.1);
    img.y = o["y"].toDouble(0.1);
    img.w = o["w"].toDouble(0.8);
    img.h = o["h"].toDouble(0.6);
    img.srcX = o["srcX"].toDouble(0.0);
    img.srcY = o["srcY"].toDouble(0.0);
    img.srcW = o["srcW"].toDouble(1.0);
    img.srcH = o["srcH"].toDouble(1.0);
    img.zOrder = o["zOrder"].toInt(0);
    img.preserveAspect = o["preserveAspect"].toBool(true);
    return img;
}

QJsonObject Slide::toJson() const {
    QJsonObject o;
    o["title"] = title;
    o["subtitle"] = subtitle;
    o["backgroundImage"] = backgroundImage;
    o["backgroundColor"] = backgroundColor;
    o["layout"] = layout;

    QJsonArray arr;
    for (const auto& b : blocks) {
        QJsonObject bo;
        bo["text"] = b.text;
        bo["type"] = b.type;
        bo["bold"] = b.bold;
        bo["italic"] = b.italic;
        bo["color"] = b.color;
        bo["fontSize"] = b.fontSize;
        arr.append(bo);
    }
    o["blocks"] = arr;

    QJsonArray imgArr;
    for (const auto& img : images) {
        imgArr.append(img.toJson());
    }
    o["images"] = imgArr;

    return o;
}

Slide Slide::fromJson(const QJsonObject& o) {
    Slide s;
    s.title = o["title"].toString();
    s.subtitle = o["subtitle"].toString();
    s.backgroundImage = o["backgroundImage"].toString();
    s.backgroundColor = o["backgroundColor"].toString();
    s.layout = o["layout"].toString("title-content");

    for (const auto& v : o["blocks"].toArray()) {
        QJsonObject bo = v.toObject();
        TextBlock b;
        b.text = bo["text"].toString();
        b.type = bo["type"].toString("p");
        b.bold = bo["bold"].toBool();
        b.italic = bo["italic"].toBool();
        b.color = bo["color"].toString();
        b.fontSize = bo["fontSize"].toInt();
        s.blocks.append(b);
    }

    for (const auto& v : o["images"].toArray()) {
        s.images.append(SlideImage::fromJson(v.toObject()));
    }

    return s;
}
#include "slide_deck.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>

SlideDeck::SlideDeck() {
    slides.append(Slide());
}

void SlideDeck::addSlide(int index) {
    Slide s;
    s.layout = "title-content";
    if (index < 0 || index >= slides.size()) slides.append(s);
    else slides.insert(index, s);
}

void SlideDeck::removeSlide(int index) {
    if (index >= 0 && index < slides.size() && slides.size() > 1)
        slides.removeAt(index);
}

void SlideDeck::duplicateSlide(int index) {
    if (index >= 0 && index < slides.size())
        slides.insert(index + 1, slides[index]);
}

bool SlideDeck::save(const QString& path) const {
    QJsonObject root;
    root["version"] = 1;
    root["name"] = name;
    root["theme"] = theme;
    QJsonArray arr;
    for (const auto& s : slides) arr.append(s.toJson());
    root["slides"] = arr;

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(QJsonDocument(root).toJson());
    return true;
}

SlideDeck SlideDeck::load(const QString& path) {
    SlideDeck d;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return d;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject()) return d;

    QJsonObject root = doc.object();
    d.name = root["name"].toString("Untitled");
    d.theme = root["theme"].toString("cosmic");
    d.slides.clear();
    for (const auto& v : root["slides"].toArray())
        d.slides.append(Slide::fromJson(v.toObject()));
    if (d.slides.isEmpty()) d.slides.append(Slide());
    return d;
}

QString SlideDeck::themeStylesheet(const QString& theme) {
    if (theme == "light") {
        return R"(
            body { background: #ffffff; color: #222; font-family: 'Georgia', serif; }
            h1 { color: #1a1a1a; font-size: 4em; margin: 0 0 0.3em 0; border-bottom: 3px solid #0066cc; padding-bottom: 0.2em; }
            h2 { color: #333; font-size: 2.5em; }
            h3 { color: #555; font-size: 1.8em; }
            p { font-size: 1.5em; line-height: 1.6; }
            ul, ol { font-size: 1.4em; line-height: 1.8; }
            .slide { padding: 8% 10%; }
        )";
    }
    if (theme == "dark") {
        return R"(
            body { background: #1a1a1a; color: #e0e0e0; font-family: 'Helvetica', sans-serif; }
            h1 { color: #ffffff; font-size: 4em; margin: 0 0 0.3em 0; border-bottom: 3px solid #ff6b00; padding-bottom: 0.2em; }
            h2 { color: #ffaa66; font-size: 2.5em; }
            h3 { color: #ffcc99; font-size: 1.8em; }
            p { font-size: 1.5em; line-height: 1.6; }
            ul, ol { font-size: 1.4em; line-height: 1.8; }
            .slide { padding: 8% 10%; }
        )";
    }
    // cosmic (default)
    return R"(
        body {
            background: radial-gradient(ellipse at top, #1a0a20 0%, #050308 100%);
            color: #e0dde8;
            font-family: 'Helvetica', 'Arial', sans-serif;
        }
        h1 {
            color: #ff2d95;
            font-size: 4em;
            margin: 0 0 0.3em 0;
            border-bottom: 3px solid #ff2d95;
            padding-bottom: 0.2em;
            text-shadow: 0 0 20px rgba(255,45,149,0.5);
        }
        h2 { color: #ff5eb0; font-size: 2.5em; text-shadow: 0 0 15px rgba(255,94,176,0.4); }
        h3 { color: #d08ad4; font-size: 1.8em; }
        p { font-size: 1.5em; line-height: 1.6; }
        ul, ol { font-size: 1.4em; line-height: 1.8; }
        li::marker { color: #00ff88; }
        a { color: #89b4fa; }
        code { color: #00ff88; font-family: 'Liberation Mono', monospace; background: rgba(0,255,136,0.1); padding: 2px 6px; border-radius: 3px; }
        .slide { padding: 8% 10%; }
    )";
}
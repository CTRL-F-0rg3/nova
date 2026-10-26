#include <QApplication>
#include <QCoreApplication>
#include <QStyleFactory>
#include "main_window.hpp"

int main(int argc, char** argv) {
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);
    app.setApplicationName("Nova");
    app.setOrganizationName("Trangorge");
    app.setStyle(QStyleFactory::create("Fusion"));
    MainWindow w;
    w.show();
    return app.exec();
}
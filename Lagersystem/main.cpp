#include "Lagersystem.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Lagersystem window;
    window.show();
    return app.exec();
}

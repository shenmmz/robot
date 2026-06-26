#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("VisualRoboticArmQtController"));
    QApplication::setOrganizationName(QStringLiteral("VisualRoboticArm"));

    MainWindow window;
    window.show();

    return app.exec();
}

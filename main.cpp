#include <ApplicationServices/ApplicationServices.h>

#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTimer>
#include <iostream>

#include "KeyPressHandler.h"

using namespace std;

int main(int argc, char* argv[]) {
    auto app = QApplication(argc, argv);
    QMenu menu;
    auto systemTrayIcon = new QSystemTrayIcon(QIcon(":/icons/trayicon.png"), &menu);
    systemTrayIcon->setContextMenu(&menu);
    systemTrayIcon->setToolTip("ImeSwitcher");
    systemTrayIcon->show();

    KeyPressHandler handler(systemTrayIcon);

    menu.addAction("init...");
    menu.addAction("Tip: Open System Settings -> General -> Login Items");
    menu.addAction("-> Plus Button, add this program to start on boot.");
    auto quitAction = menu.addAction("Quit");
    QObject::connect(quitAction, &QAction::triggered, &app, [&app] { app.quit(); });

    handler.init();

    return app.exec();
}

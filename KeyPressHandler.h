#ifndef KEYPRESSHANDLER_H
#define KEYPRESSHANDLER_H

#include <ApplicationServices/ApplicationServices.h>

#include <QSystemTrayIcon>
#include <set>

class KeyPressHandler {
   public:
    KeyPressHandler(QSystemTrayIcon*);
    ~KeyPressHandler();
    void init();

    void handleEvent(CGEventType type, CGEventRef event);

   private:
    QSystemTrayIcon *systemTrayIcon = NULL;
    __CFMachPort* m_eventTapPtr;
    int initCount = 0;
    bool flagCheckEnabled = true;
    CGEventFlags modifiers = 0;
    std::set<int64_t> keys;
};

#endif  // KEYPRESSHANDLER_H

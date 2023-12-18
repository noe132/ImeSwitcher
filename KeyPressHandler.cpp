#include "KeyPressHandler.h"

#include <Carbon/Carbon.h>

#include <QMenu>
#include <QSystemTrayIcon>
#include <QTimer>
#include <iostream>

using namespace std;

CGEventFlags MODIFIER_CHECK_MASK = NX_DEVICELCTLKEYMASK | NX_DEVICELSHIFTKEYMASK | NX_DEVICERSHIFTKEYMASK |
                                   NX_DEVICELCMDKEYMASK | NX_DEVICERCMDKEYMASK | NX_DEVICELALTKEYMASK |
                                   NX_DEVICERALTKEYMASK | NX_DEVICERCTLKEYMASK;

CGEventFlags LCMD_LSHIFT_MASK = NX_DEVICELCMDKEYMASK | NX_DEVICELSHIFTKEYMASK;

std::string toString(CFStringRef input) {
    if (!input) return {};

    // Attempt to access the underlying buffer directly. This only works if no
    // conversion or
    //  internal allocation is required.
    auto originalBuffer{CFStringGetCStringPtr(input, kCFStringEncodingUTF8)};
    if (originalBuffer) return originalBuffer;

    // Copy the data out to a local buffer.
    auto lengthInUtf16{CFStringGetLength(input)};
    auto maxLengthInUtf8{
        CFStringGetMaximumSizeForEncoding(lengthInUtf16, kCFStringEncodingUTF8) + 1
    };  // <-- leave room for null terminator
    std::vector<char> localBuffer(maxLengthInUtf8);

    if (CFStringGetCString(input, localBuffer.data(), maxLengthInUtf8, kCFStringEncodingUTF8))
        return localBuffer.data();

    return {};
}

void switchIme() {
    auto sources = TISCreateInputSourceList(NULL, false);
    auto count = CFArrayGetCount(sources);

    std::vector<TISInputSourceRef> vec;

    std::cout << "------------------------------" << endl;
    for (int i = 0; i < count; i += 1) {
        auto source = (TISInputSourceRef)CFArrayGetValueAtIndex(sources, i);
        auto category = (CFStringRef)TISGetInputSourceProperty(source, kTISPropertyInputSourceCategory);
        auto type = (CFStringRef)TISGetInputSourceProperty(source, kTISPropertyInputSourceType);
        auto isInputSource = category == kTISCategoryKeyboardInputSource;
        auto isIme = type == kTISTypeKeyboardInputMode || type == kTISTypeKeyboardLayout;
        if (isInputSource && isIme) {
            vec.push_back(source);
        }
    }

    auto currentSource = TISCopyCurrentKeyboardInputSource();
    auto currentSourceId = (CFStringRef)TISGetInputSourceProperty(currentSource, kTISPropertyInputSourceID);

    auto it = find_if(vec.begin(), vec.end(), [&currentSourceId](const auto v) {
        auto id = (CFStringRef)TISGetInputSourceProperty(v, kTISPropertyInputSourceID);
        return id == currentSourceId;
    });
    if (it == vec.end()) {
        std::cout << "notfound" << endl;
        return;
    }
    if (it + 1 == vec.end()) {
        it = vec.begin();
    } else {
        it += 1;
    }

    auto source = *it;

    auto sourceName = (CFStringRef)TISGetInputSourceProperty(source, kTISPropertyLocalizedName);
    auto id = (CFStringRef)TISGetInputSourceProperty(source, kTISPropertyInputSourceID);

    std::cout << "switch to: " << toString(sourceName) << endl;
    std::cout << "id: " << toString(id) << endl;

    // switch to source
    TISSelectInputSource(source);
}

KeyPressHandler::KeyPressHandler(QSystemTrayIcon* icon) { systemTrayIcon = icon; }

KeyPressHandler::~KeyPressHandler() {}

void KeyPressHandler::init() {
    CGEventMask keyEventMask =
        CGEventMaskBit(kCGEventFlagsChanged) | CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventKeyUp);

    m_eventTapPtr = CGEventTapCreate(
        kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionListenOnly, keyEventMask,
        [](CGEventTapProxy, CGEventType type, CGEventRef event, void* keyPressHandlerPtr) {
            static_cast<KeyPressHandler*>(keyPressHandlerPtr)->handleEvent(type, event);
            return event;
        },
        this
    );

    auto menu = systemTrayIcon->contextMenu();
    auto actions = menu->actions();
    auto theAction = actions.at(0);

    if (m_eventTapPtr == nullptr) {
        initCount += 1;
        auto tooltip = "Initing..." + ((std::string)"...").substr(0, initCount % 3);
        theAction->setText(QString::fromStdString(tooltip));
        QTimer::singleShot(1000, [this]() { this->init(); });
        return;
    }

    theAction->setText("Init Success!");

    CFRunLoopAddSource(
        CFRunLoopGetCurrent(), CFMachPortCreateRunLoopSource(kCFAllocatorDefault, m_eventTapPtr, 0),
        kCFRunLoopCommonModes
    );

    CGEventTapEnable(m_eventTapPtr, true);
}

void KeyPressHandler::handleEvent(CGEventType type, CGEventRef event) {
    if (type == kCGEventFlagsChanged) {
        auto newFlags = CGEventGetFlags(event) & MODIFIER_CHECK_MASK;
        bool lcmd_or_lshift = newFlags == NX_DEVICELCMDKEYMASK || newFlags == NX_DEVICELSHIFTKEYMASK;
        if (flagCheckEnabled && modifiers == LCMD_LSHIFT_MASK && lcmd_or_lshift) {
            switchIme();
        }
        auto without_lcmd_lshift = newFlags & ~LCMD_LSHIFT_MASK;
        if (without_lcmd_lshift != 0) {
            flagCheckEnabled = false;
        }
        if (keys.size() == 0 && newFlags == 0) {
            flagCheckEnabled = true;
        }
        modifiers = newFlags;
    } else if (type == kCGEventKeyUp) {
        auto key = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
        keys.erase(key);
        if (keys.size() == 0 && modifiers == 0) {
            flagCheckEnabled = true;
        }
    } else if (type == kCGEventKeyDown) {
        auto key = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
        keys.insert(key);
        flagCheckEnabled = false;
    }
    return;
}

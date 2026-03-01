#include "Error.hpp"

#include <QCoreApplication>

// TODO
[[noreturn]] void Qt::App::Error::Exit(const std::string& Message) {
    if (QCoreApplication::instance() != nullptr) {
        // Emit on the GUI thread to open the popup
        // QMetaObject::invokeMethod(
        //     &GuiErrorBridge::Instance(),
        //     [QtMessage]() {
        //         emit GuiErrorBridge::Instance().FatalErrorOccurred(QtMessage);
        //     },
        //     Qt::QueuedConnection);

        // Let the event loop process the dialog, then quit.
        // Hard-quit from here to guarantee exit after user closes dialog.
        QMetaObject::invokeMethod(
            QCoreApplication::instance(),
            []() { QCoreApplication::exit(1); },
            Qt::QueuedConnection);

        // Block forever; control never returns.
        // QThread::currentThread()->eventDispatcher()->processEvents(
        //     QEventLoop::AllEvents);
    }
    (void)Message; // Placeholder to prevent unused-var warning
    std::abort();
}

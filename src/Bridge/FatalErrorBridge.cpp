#include "FatalErrorBridge.hpp"

#include <QCoreApplication>

namespace Bridge {

FatalErrorBridge::FatalErrorBridge(QObject* Parent)
    : QObject(Parent) {
}

QString FatalErrorBridge::Message() const {
    return m_Message;
}

void FatalErrorBridge::RequestFatalError(QString&& Message, int ExitCode) {
    m_Message = std::move(Message);
    m_ExitCode = ExitCode;
    emit messageChanged();
    emit showFatalDialog();
}

void FatalErrorBridge::AcknowledgeAndExit() {
    QCoreApplication::exit(m_ExitCode);
}

}

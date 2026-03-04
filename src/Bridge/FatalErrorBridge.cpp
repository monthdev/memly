#include "FatalErrorBridge.hpp"

#include <QCoreApplication>
#include <utility>

FatalErrorBridge& FatalErrorBridge::Instance() {
    static FatalErrorBridge s_Instance;
    return s_Instance;
}

FatalErrorBridge::FatalErrorBridge(QObject* Parent)
    : QObject(Parent) {
}

void FatalErrorBridge::RequestFatalError(QString Message, int ExitCode) {
    m_Message = std::move(Message);
    m_ExitCode = ExitCode;
    emit messageChanged();
    emit showFatalDialog();
}

void FatalErrorBridge::AcknowledgeAndExit() {
    QCoreApplication::exit(m_ExitCode);
}

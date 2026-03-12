#pragma once

#include <QObject>

namespace Bridge {

class FatalErrorBridge final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString message READ Message NOTIFY messageChanged)

public:
    explicit FatalErrorBridge(QObject* Parent = nullptr);

    QString Message() const;
    void RequestFatalError(QString&& Message, int ExitCode);

public slots:
    void AcknowledgeAndExit();

signals:
    void messageChanged();
    void showFatalDialog();

private:
    QString m_Message;
    int m_ExitCode{ 1 };
};

}

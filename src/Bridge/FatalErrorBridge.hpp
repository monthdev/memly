#pragma once

#include <QObject>
#include <QTimer>

class FatalErrorBridge final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString message READ Message NOTIFY messageChanged)

public:
    static FatalErrorBridge& Instance();

    void RequestFatalError(QString Message, int ExitCode);

    QString Message() const {
        return m_Message;
    }

public slots:
    void AcknowledgeAndExit();

signals:
    void showFatalDialog();
    void messageChanged();

private:
    explicit FatalErrorBridge(QObject* Parent = nullptr);

    QString m_Message;
    int m_ExitCode{ 1 };
};

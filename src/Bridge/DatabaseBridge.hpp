// #pragma once
//
// #include <duckdb.hpp>
//
// #include <QObject>
// #include <QString>
//
// class DatabaseBridge final : public QObject {
//     Q_OBJECT
//     Q_PROPERTY(QString lastError READ LastError NOTIFY lastErrorChanged)
//
// public:
//     static DatabaseBridge& Instance();
//
//     void Initialize(QString DatabaseFilePath);
//
//     QString LastError() const {
//         return m_LastError;
//     }
//
//     // Q_INVOKABLE bool AddCard(QString Front, QString Back);
//     // Q_INVOKABLE bool UpdateCard(qint64 CardId, QString Front, QString Back);
//     // Q_INVOKABLE bool DeleteCard(qint64 CardId);
//
// signals:
//     void lastErrorChanged();
//
//     // void cardAdded(qint64 CardId);
//     // void cardUpdated(qint64 CardId);
//     // void cardDeleted(qint64 CardId);
//
// private:
//     explicit DatabaseBridge(QObject* Parent = nullptr);
//
//     template <typename Fn>
//     bool InvokeNoThrow(Fn&& Function);
//
//     void SetLastError(QString Error);
//
//     QString m_LastError;
//
//     bool m_Initialized{ false };
// };

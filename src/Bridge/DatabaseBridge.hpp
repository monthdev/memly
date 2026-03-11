// #pragma once
//
// #include <duckdb.hpp>
//
// #include <QObject>
// #include <QString>
//
// class DatabaseBridge final : public QObject {
//     Q_OBJECT
//     // Q_PROPERTY(QString lastError READ LastError NOTIFY lastErrorChanged)
//
// public:
//     static DatabaseBridge& Instance();
//
//     void Initialize(QString DatabaseFilePath);
//
//     // QString LastError() const {
//     //     return m_LastError;
//     // }
//
//     Q_INVOKABLE bool AddDeck(const QString& Name);
//
// private:
//     explicit DatabaseBridge(QObject* Parent = nullptr);
//
//     template <typename Fn>
//     bool InvokeNoThrow(Fn&& Function);
//
//     // void SetLastError(QString Error);
//
//     // QString m_LastError;
//
//     // bool m_Initialized{ false };
// };

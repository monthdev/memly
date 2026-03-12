// #pragma once
//
// #include <duckdb.hpp>
//
// #include <QObject>
// #include <QString>
//
// class DatabaseBridge final : public QObject {
//     Q_OBJECT
//
// public:
//     explicit DatabaseBridge(duckdb::Connection& Connection,
//                             QObject* Parent = nullptr);
//
//     Q_INVOKABLE bool AddDeck(const QString& Name);
//
// private:
//     duckdb::Connection& m_Connection;
//
//     template <typename Fn>
//     bool InvokeNoThrow(Fn&& Function);
// };

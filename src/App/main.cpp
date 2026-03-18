#include <duckdb.hpp>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "App/AppData.hpp"
#include "Bridge/DeckBridge.hpp"
#include "Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

int main(int argc, char* argv[]) {
    return Support::TryCatchWrapper([&] {
        Q_INIT_RESOURCE(Sql);
        QGuiApplication App{ argc, argv };
        constexpr const char* AppName{ "Memly" };
        constexpr const char* OrgName{ "monthly" };
        App.setApplicationName(AppName);
        App.setApplicationDisplayName(AppName);
        App.setOrganizationDomain(OrgName);
        App.setOrganizationName(OrgName);
        duckdb::DuckDB Database{ App::DatabaseFilePath().toStdString() };
        duckdb::Connection DatabaseConnection{ Database };

        // TODO: Schema migrator here
        std::unique_ptr<duckdb::QueryResult> QueryResult{
            DatabaseConnection.Query(Sql::InitialSchemaSql())
        };
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }

        Bridge::DeckBridge DeckBridge{ DatabaseConnection, &App };
        qmlRegisterSingletonInstance(AppName, 1, 0, "DeckBridge", &DeckBridge);
        QQmlApplicationEngine AppEngine{};
        QObject::connect(
            &AppEngine,
            &QQmlApplicationEngine::objectCreationFailed,
            QCoreApplication::instance(),
            [] { QCoreApplication::exit(-1); },
            Qt::QueuedConnection);
        AppEngine.loadFromModule(AppName, "MainWindow");
        return App.exec();
    });
}

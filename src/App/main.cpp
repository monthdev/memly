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
        const QString AppName{ "Memly" };
        App.setApplicationName(AppName);
        App.setApplicationDisplayName(AppName);
        QQmlApplicationEngine AppEngine{};
        duckdb::DuckDB Database{ App::DatabaseFilePath().toStdString() };
        duckdb::Connection DatabaseConnection{ Database };

        // TODO: Schema migrator here
        std::unique_ptr<duckdb::QueryResult> QueryResult{
            DatabaseConnection.Query(Sql::InitialSchema())
        };
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }

        Bridge::DeckBridge DeckBridge{ DatabaseConnection };
        AppEngine.rootContext()->setContextProperty("DeckBridge", &DeckBridge);
        QObject::connect(
            &AppEngine,
            &QQmlApplicationEngine::objectCreationFailed,
            QCoreApplication::instance(),
            [] { QCoreApplication::exit(-1); },
            Qt::QueuedConnection);
        AppEngine.loadFromModule("Memly", "MainWindow");

        return App.exec();
    });
}

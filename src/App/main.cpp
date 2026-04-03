#include <duckdb.hpp>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "App/AppData.hpp"
#include "Controller/DeckListController.hpp"
#include "Service/DeckListService.hpp"
#include "Sql/SchemaMigrator.hpp"
#include "Support/Fatal.hpp"

int main(int argc, char* argv[]) {
    return Support::TryCatchWrapper([&] {
        Q_INIT_RESOURCE(Sql);
        QGuiApplication App{ argc, argv };
        constexpr char AppName[]{ "Memly" };
        constexpr char OrgName[]{ "monthly" };
        App.setApplicationName(AppName);
        App.setApplicationDisplayName(AppName);
        App.setOrganizationDomain(OrgName);
        App.setOrganizationName(OrgName);
        duckdb::DuckDB Database{ App::DatabaseFilePath().toStdString() };
        duckdb::Connection DatabaseConnection{ Database };
        Sql::RunDatabaseBootstrap(DatabaseConnection);
        Service::DeckListService DeckListService{ DatabaseConnection, &App };
        qmlRegisterSingletonInstance(AppName, 1, 0, "DeckListService", &DeckListService);
        qmlRegisterType<Controller::DeckListController>(AppName, 1, 0, "DeckListController");
        QQmlApplicationEngine AppEngine{};
        QObject::connect(
            &AppEngine,
            &QQmlApplicationEngine::objectCreationFailed,
            QCoreApplication::instance(),
            [] { QCoreApplication::exit(-1); },
            Qt::QueuedConnection);
        AppEngine.loadFromModule(AppName, "ApplicationWindow");
        return App.exec();
    });
}

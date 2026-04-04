#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "App/AppData.hpp"
#include "App/Bootstrap/AppContext.hpp"
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
        App::AppContext::Initialize(App::DatabaseFilePath());
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

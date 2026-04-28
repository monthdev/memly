#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "Bootstrap/AppContext.hpp"
#include "Support/AppStoragePath.hpp"
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
        Bootstrap::AppContext::Initialize(Support::AppStoragePath::DatabaseFilePath());
        QQmlApplicationEngine AppEngine{};
        QObject::connect(
            &AppEngine, &QQmlApplicationEngine::objectCreationFailed, QCoreApplication::instance(), [] { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
        AppEngine.loadFromModule(AppName, "MainWindow");
        return App.exec();
    });
}

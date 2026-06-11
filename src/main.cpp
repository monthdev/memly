#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "Bootstrap/RuntimeContext.hpp"
#include "Runtime/AppStoragePath.hpp"
#include "Runtime/Crash.hpp"

int main(int argc, char* argv[]) {
    return Runtime::TryCatchWrapper([&]() -> int {
        Q_INIT_RESOURCE(Sql);
        QGuiApplication App{ argc, argv };
        constexpr char AppName[]{ "Memly" };
        constexpr char OrgName[]{ "MemlyProject" };
        App.setApplicationName(AppName);
        App.setApplicationDisplayName(AppName);
        App.setOrganizationDomain(OrgName);
        App.setOrganizationName(OrgName);
        Bootstrap::RuntimeContext::Initialize(Runtime::DatabaseFilePath());
        QQmlApplicationEngine AppEngine{};
        QObject::connect(
            &AppEngine,
            &QQmlApplicationEngine::objectCreationFailed,
            QCoreApplication::instance(),
            []() -> void { QCoreApplication::exit(-1); },
            Qt::QueuedConnection);
        AppEngine.loadFromModule(AppName, "MainWindow");
        return App.exec();
    });
}

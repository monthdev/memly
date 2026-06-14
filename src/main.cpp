#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <string_view>

#include "Bootstrap/RuntimeContext.hpp"
#include "Runtime/AppStoragePath.hpp"
#include "Runtime/Crash.hpp"

int main(int argc, char* argv[]) noexcept {
    return Runtime::TryCatchWrapper([&]() -> int {
        Q_INIT_RESOURCE(Sql);
        QGuiApplication App{ argc, argv };
        constexpr std::string_view AppName{ "Memly" };
        constexpr std::string_view OrgName{ "MemlyProject" };
        App.setApplicationName(AppName.data());
        App.setApplicationDisplayName(AppName.data());
        App.setOrganizationDomain(OrgName.data());
        App.setOrganizationName(OrgName.data());
        Bootstrap::RuntimeContext::Initialize(Runtime::DatabaseFilePath());
        QQmlApplicationEngine AppEngine{};
        QObject::connect(
            &AppEngine,
            &QQmlApplicationEngine::objectCreationFailed,
            QCoreApplication::instance(),
            []() -> void { QCoreApplication::exit(-1); },
            Qt::QueuedConnection);
        AppEngine.loadFromModule(AppName.data(), "MainWindow");
        return App.exec();
    });
}

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <string_view>

#include "Bootstrap/RuntimeContext.hpp"
#include "Support/Runtime/ExceptionBoundary.hpp"
#include "Support/Runtime/QtApp/QtAppStoragePath.hpp"

int main(int argc, char* argv[]) noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> int {
        Q_INIT_RESOURCE(Sql);
        QGuiApplication App{ argc, argv };
        constexpr std::string_view AppName{ "Memly" };
        constexpr std::string_view OrgName{ "MemlyProject" };
        App.setApplicationName(AppName.data());
        App.setApplicationDisplayName(AppName.data());
        App.setOrganizationDomain(OrgName.data());
        App.setOrganizationName(OrgName.data());
        Bootstrap::RuntimeContext::Initialize(Support::Runtime::QtApp::DatabaseFilePath());
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

#include <qcoreapplication.h>
#include <qguiapplication.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlapplicationengine.h>
#include <qtresource.h>

#include "Bootstrap/RuntimeContext.hpp"
#include "Support/Runtime/ExceptionBoundary.hpp"
#include "Support/Runtime/QtApp/QtAppStoragePath.hpp"

int main(int argc, char* argv[]) noexcept {
    return Support::Runtime::TryCatchWrapper([&]() -> int {
        Q_INIT_RESOURCE(Sql);
        const QGuiApplication App{ argc, argv };
        constexpr const char* const AppName{ "Memly" };
        constexpr const char* const OrgName{ "MemlyProject" };
        App.setApplicationName(AppName);
        App.setApplicationDisplayName(AppName);
        App.setOrganizationDomain(OrgName);
        App.setOrganizationName(OrgName);
        Bootstrap::RuntimeContext::Initialize(Support::Runtime::QtApp::DatabaseFilePath());
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

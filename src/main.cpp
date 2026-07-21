#include <qcoreapplication.h>
#include <qguiapplication.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlapplicationengine.h>
#include <qtresource.h>

#include "Bootstrap/RuntimeContext.hpp"
#include "Support/Runtime/ExceptionBoundary.hpp"
#include "Support/Runtime/QtApp/QtAppStoragePath.hpp"

auto main(int argc, char** argv) noexcept -> int {
    return Support::Runtime::TryCatchWrapper([&]() -> int {
        Q_INIT_RESOURCE(Sql);
        [[maybe_unused]] const QGuiApplication QtApplicationLifetime{ argc, argv };
        constexpr const char* AppName{ "Memly" };
        constexpr const char* OrgName{ "MemlyProject" };
        QCoreApplication::setApplicationName(AppName);
        QGuiApplication::setApplicationDisplayName(AppName);
        QCoreApplication::setOrganizationDomain(OrgName);
        QCoreApplication::setOrganizationName(OrgName);
        Bootstrap::RuntimeContext::Initialize(Support::Runtime::QtApp::DatabaseFilePath());
        QQmlApplicationEngine AppEngine{};
        QObject::connect(
            &AppEngine,
            &QQmlApplicationEngine::objectCreationFailed,
            QCoreApplication::instance(),
            []() -> void { QCoreApplication::exit(-1); },
            Qt::QueuedConnection);
        AppEngine.loadFromModule(AppName, "MainWindow");
        return QGuiApplication::exec();
    });
}

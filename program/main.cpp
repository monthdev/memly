#include <qcoreapplication.h>
#include <qguiapplication.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlapplicationengine.h>
#include <qtresource.h>

#include <cstdlib>

#include "CompositionRoot/RuntimeContext.hpp"
#include "Support/Runtime/Exception/ExceptionBoundary.hpp"
#include "Support/Runtime/QtApp/QtAppStoragePath.hpp"

auto main(int argc, char** argv) noexcept -> int {
    return int{ Support::Runtime::Exception::TryCatchWrapper([&]() -> int {
        Q_INIT_RESOURCE(DuckDbMigrationSql);
        Q_INIT_RESOURCE(DuckDbSeedSql);
        Q_INIT_RESOURCE(DuckDbDeckSql);
        [[maybe_unused]] const QGuiApplication QtApplicationLifetime{ argc, argv };
        constexpr const char* AppName{ "Memly" };
        constexpr const char* OrgName{ "MemlyProject" };
        QCoreApplication::setApplicationName(AppName);
        QGuiApplication::setApplicationDisplayName(AppName);
        QCoreApplication::setOrganizationDomain(OrgName);
        QCoreApplication::setOrganizationName(OrgName);
        CompositionRoot::RuntimeContext::Initialize(Support::Runtime::QtApp::DatabaseFilePath());
        QQmlApplicationEngine AppEngine{};
        QObject::connect(
            &AppEngine,
            &QQmlApplicationEngine::objectCreationFailed,
            QCoreApplication::instance(),
            []() -> void { QCoreApplication::exit(EXIT_FAILURE); },
            Qt::QueuedConnection);
        AppEngine.loadFromModule(AppName, "MainWindow");
        return int{ QGuiApplication::exec() };
    }) };
}

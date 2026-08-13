#include <qbytearrayview.h>
#include <qcoreapplication.h>
#include <qguiapplication.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qqmlapplicationengine.h>
#include <qtresource.h>

#include <cstdlib>

#include "Memly/CompositionRoot/ApplicationRuntime.hpp"
#include "Memly/Exception/ExceptionBoundary.hpp"
#include "Memly/QtApp/QtAppStoragePath.hpp"

auto main(int argc, char** argv) noexcept -> int {
    return int{ Support::Exception::TryCatchWrapper([&]() -> int {
        Q_INIT_RESOURCE(DatabaseMigrationSql);
        Q_INIT_RESOURCE(RepositoryDeckSql);
        [[maybe_unused]] const QGuiApplication QtApplicationLifetime{ argc, argv };
        constexpr const char* AppName{ "Memly" };
        constexpr const char* OrgName{ "MemlyProject" };
        QCoreApplication::setApplicationName(AppName);
        QGuiApplication::setApplicationDisplayName(AppName);
        QCoreApplication::setOrganizationDomain(OrgName);
        QCoreApplication::setOrganizationName(OrgName);
        [[maybe_unused]] const CompositionRoot::ApplicationRuntime ApplicationRuntime{ Support::QtApp::DatabaseFilePath() };
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

#include <qbytearrayview.h>
#include <qcoreapplication.h>
#include <qguiapplication.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qqmlapplicationengine.h>
#include <qtresource.h>

#include <cstdlib>

import Memly.CompositionRoot;
import Memly.Exception;
import Memly.QtApp;

auto main(int argc, char** argv) noexcept -> int {
    return int{ Memly::Exception::TryCatchWrapper([&]() -> int {
        Q_INIT_RESOURCE(DatabaseMigrationSql);
        Q_INIT_RESOURCE(RepositoryDeckSql);
        [[maybe_unused]] const QGuiApplication QtApplicationLifetime{ argc, argv };
        constexpr const char* AppName{ "Memly" };
        constexpr const char* OrgName{ "MemlyProject" };
        QCoreApplication::setApplicationName(AppName);
        QGuiApplication::setApplicationDisplayName(AppName);
        QCoreApplication::setOrganizationDomain(OrgName);
        QCoreApplication::setOrganizationName(OrgName);
        [[maybe_unused]] const Memly::CompositionRoot::ApplicationRuntime ApplicationRuntime{ Memly::QtApp::DatabaseFilePath() };
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

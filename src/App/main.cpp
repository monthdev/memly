#include <duckdb.hpp>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Bridge/FatalErrorBridge.hpp"
#include "Bridge/SqlResource.hpp"
#include "Bridge/SupportData.hpp"

int main(int argc, char* argv[]) {
    Q_INIT_RESOURCE(Sql);
    try {
        QGuiApplication App{ argc, argv };
        const std::string AppName{ "Memly" };
        App.setApplicationName(AppName.c_str());
        App.setApplicationDisplayName(AppName.c_str());
        QQmlApplicationEngine AppEngine{};
        FatalErrorBridge::Instance().moveToThread(App.thread());
        AppEngine.rootContext()->setContextProperty(
            "FatalErrorBridge", &FatalErrorBridge::Instance());
        QObject::connect(
            &AppEngine,
            &QQmlApplicationEngine::objectCreationFailed,
            QCoreApplication::instance(),
            [] { QCoreApplication::exit(-1); },
            Qt::QueuedConnection);
        AppEngine.loadFromModule("Memly", "MainWindow");

        std::cout << DatabaseFilePath() << "\n";
        duckdb::DuckDB Database{ DatabaseFilePath() };
        duckdb::Connection Connection{ Database };
        auto Result{ Connection.Query(InitialSchemaSql()) };
        auto ErrorType{ Result->GetErrorType() };
        std::cout << static_cast<int>(ErrorType) << "\n";
        auto ErrorObject{ Result->GetErrorObject() };
        ErrorObject.ConvertErrorToJSON();
        std::cout << ErrorObject.Message() << "\n";

        auto Result1{ Connection.Query(
            "insert into decks (name) values('deutsch');") };
        auto ErrorType1{ Result1->GetErrorType() };
        std::cout << static_cast<int>(ErrorType1) << "\n";
        auto ErrorObject1{ Result1->GetErrorObject() };
        ErrorObject1.ConvertErrorToJSON();
        std::cout << ErrorObject1.Message() << "\n";
        for (const auto& [Key, Value] : ErrorObject1.ExtraInfo()) {
            std::cout << Key << ": " << Value << "\n";
        }
        QTimer::singleShot(0, [] {
            FatalErrorBridge::Instance().RequestFatalError("Test fatal dialog",
                                                           1);
        });
        return App.exec();
    } catch (const std::exception& Exception) {
        qFatal("%s", Exception.what());
    } catch (...) { qFatal("Unknown exception"); }
}

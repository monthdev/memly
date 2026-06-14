#include <QCoreApplication>
#include <QTest>
#include <string_view>

int main(int argc, char* argv[]) {
    constexpr std::string_view AppName{ "Memly" };
    QCoreApplication::setApplicationName(AppName.data());
    QCoreApplication App(argc, argv);

    int ExitCode{ 0 };

    //     {
    //         TestSqlResource TestObject;
    //         ExitCode |= QTest::qExec(&TestObject, argc, argv);
    //     }
    //
    // #if defined(__APPLE__)
    //     {
    //         TestSupportDataMacOS TestObject;
    //         ExitCode |= QTest::qExec(&TestObject, argc, argv);
    //     }
    // #endif

    return ExitCode;
}

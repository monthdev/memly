#include <QCoreApplication>
#include <QTest>

int main(int argc, char* argv[]) {
    QCoreApplication::setApplicationName("Memly");
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

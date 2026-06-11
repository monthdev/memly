#pragma once

#include <QString>

namespace Runtime {
[[nodiscard]] QString AudioDirectoryPath();

[[nodiscard]] QString CrashLogFilePath();

[[nodiscard]] QString DatabaseFilePath();
}

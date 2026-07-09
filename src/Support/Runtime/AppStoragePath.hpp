#pragma once

#include <QString>

namespace Support::Runtime {
[[nodiscard]] QString AudioDirectoryPath();

[[nodiscard]] QString CrashLogFilePath();

[[nodiscard]] QString DatabaseFilePath();
}

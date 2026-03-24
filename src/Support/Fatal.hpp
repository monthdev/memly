#pragma once

#include <QtLogging>
#include <functional>
#include <source_location>
#include <string>

namespace Support {
[[noreturn]] void ThrowError(const std::string& = {},
                             const std::source_location& = std::source_location::current());

void LogError(const std::string&);

template <typename Fn>
decltype(auto) TryCatchWrapper(Fn&& Function) noexcept {
    try {
        return std::invoke(std::forward<Fn>(Function));
    } catch (const std::exception& Exception) {
        Support::LogError(Exception.what());
        qFatal("%s", Exception.what());
    } catch (...) {
        constexpr char What[]{ "Unknown exception" };
        Support::LogError(What);
        qFatal("%s", What);
    }
}
}

#pragma once

#include <QtLogging>
#include <functional>
#include <source_location>
#include <string_view>

namespace Runtime {
[[noreturn]] void ThrowError(const std::string_view = std::string_view{}, const std::source_location& = std::source_location::current());

void LogError(const std::string_view);

template <typename Fn>
decltype(auto) TryCatchWrapper(Fn&& Function) noexcept {
    try {
        return std::invoke(std::forward<Fn>(Function));
    } catch (const std::exception& Exception) {
        Runtime::LogError(Exception.what());
        qFatal("%s", Exception.what());
    } catch (...) {
        constexpr char What[]{ "Unknown exception" };
        Runtime::LogError(What);
        qFatal("%s", What);
    }
}
}

#pragma once

#include <QtLogging>
#include <concepts>
#include <functional>
#include <source_location>
#include <string_view>
#include <type_traits>

namespace Runtime {
[[noreturn]] void ThrowError(const std::string_view = std::string_view{}, const std::source_location& = std::source_location::current());

void LogError(const std::string_view);

template <typename FunctionType>
    requires std::invocable<FunctionType&&>
std::invoke_result_t<FunctionType&&> TryCatchWrapper(FunctionType&& Function) noexcept {
    try {
        return std::invoke(std::forward<FunctionType>(Function));
    } catch (const std::exception& Exception) {
        Runtime::LogError(Exception.what());
        qFatal("%s", Exception.what());
    } catch (...) {
        constexpr std::string_view What{ "Unknown exception" };
        Runtime::LogError(What);
        qFatal("%s", What.data());
    }
}
}

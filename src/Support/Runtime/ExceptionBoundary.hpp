#pragma once

#include <concepts>
#include <cstdlib>
#include <exception>
#include <functional>
#include <string_view>
#include <type_traits>
#include <utility>

namespace Support::Runtime {
void LogException(std::string_view) noexcept;

template <typename FunctionType>
    requires std::invocable<FunctionType&&>
[[nodiscard]] auto TryCatchWrapper(FunctionType&& Function) noexcept -> std::invoke_result_t<FunctionType&&> {
    try {
        return std::invoke(std::forward<FunctionType>(Function));
    } catch (const std::exception& Exception) {
        LogException(Exception.what());
        std::abort();
    } catch (...) {
        LogException("Unknown exception caught");
        std::abort();
    }
}
}

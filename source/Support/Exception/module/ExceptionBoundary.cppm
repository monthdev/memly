module;

#include <concepts>
#include <cstdlib>
#include <exception>
#include <functional>
#include <string_view>
#include <type_traits>
#include <utility>

export module Memly.Exception.ExceptionBoundary;

export namespace Memly::Exception {
void LogException(std::string_view) noexcept;

template <typename LambdaType>
    requires std::invocable<LambdaType&&>
[[nodiscard]] std::invoke_result_t<LambdaType&&>
TryCatchWrapper(LambdaType&& Lambda) noexcept {
    try {
        return std::invoke(std::forward<LambdaType>(Lambda));
    } catch (const std::exception& CaughtException) {
        LogException(CaughtException.what());
        std::abort();
    } catch (...) {
        LogException("Non-standard exception caught");
        std::abort();
    }
}
}

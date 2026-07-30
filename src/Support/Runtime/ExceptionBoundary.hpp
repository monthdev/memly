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

template <typename LambdaType>
    requires std::invocable<LambdaType&&>
[[nodiscard]] auto TryCatchWrapper(LambdaType&& Lambda) noexcept -> std::invoke_result_t<LambdaType&&> {
    try {
        return std::invoke(std::forward<LambdaType>(Lambda));
    } catch (const std::exception& Exception) {
        LogException(Exception.what());
        std::abort();
    } catch (...) {
        LogException("Non-standard exception caught");
        std::abort();
    }
}
}

#pragma once

#include <concepts>
#include <cstdlib>
#include <exception>
#include <functional>
#include <string_view>
#include <type_traits>
#include <utility>

namespace Support::Runtime {
void LogException(const std::string_view) noexcept;

template <typename FunctionType>
    requires std::invocable<FunctionType&&>
std::invoke_result_t<FunctionType&&> TryCatchWrapper(FunctionType&& Function) noexcept {
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

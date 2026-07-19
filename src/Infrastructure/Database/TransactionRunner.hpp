#pragma once

#include <duckdb.hpp>

#include <concepts>
#include <expected>
#include <functional>
#include <type_traits>
#include <utility>

namespace Infrastructure::Database {

class TransactionRunner final {
private:
    template <typename>
    struct IsStdExpectedType : std::false_type {
        explicit constexpr IsStdExpectedType() noexcept
            : std::false_type{} {
        }

        explicit IsStdExpectedType(const IsStdExpectedType&) = delete;
        explicit IsStdExpectedType(IsStdExpectedType&&) = delete;
        auto operator=(const IsStdExpectedType&) -> IsStdExpectedType& = delete;
        auto operator=(IsStdExpectedType&&) -> IsStdExpectedType& = delete;
    };

    template <typename SuccessType, typename ErrorType>
    struct IsStdExpectedType<std::expected<SuccessType, ErrorType>> : std::true_type {
        explicit constexpr IsStdExpectedType() noexcept
            : std::true_type{} {
        }

        explicit IsStdExpectedType(const IsStdExpectedType&) = delete;
        explicit IsStdExpectedType(IsStdExpectedType&&) = delete;
        auto operator=(const IsStdExpectedType&) -> IsStdExpectedType& = delete;
        auto operator=(IsStdExpectedType&&) -> IsStdExpectedType& = delete;
    };

    template <typename>
    struct AlwaysFalseType : std::false_type {
        explicit constexpr AlwaysFalseType() noexcept
            : std::false_type{} {
        }

        explicit AlwaysFalseType(const AlwaysFalseType&) = delete;
        explicit AlwaysFalseType(AlwaysFalseType&&) = delete;
        auto operator=(const AlwaysFalseType&) -> AlwaysFalseType& = delete;
        auto operator=(AlwaysFalseType&&) -> AlwaysFalseType& = delete;
    };

    duckdb::Connection& m_DatabaseConnection;

public:
    explicit TransactionRunner(duckdb::Connection& DatabaseConnection) noexcept
        : m_DatabaseConnection{ DatabaseConnection } {
    }

    explicit TransactionRunner(const TransactionRunner&) = delete;
    explicit TransactionRunner(TransactionRunner&&) = delete;
    auto operator=(const TransactionRunner&) -> TransactionRunner& = delete;
    auto operator=(TransactionRunner&&) -> TransactionRunner& = delete;

    template <typename ServiceMethodType>
        requires std::invocable<ServiceMethodType&&>
    [[nodiscard]] auto TransactionWrapper(ServiceMethodType&& ServiceMethod) -> std::invoke_result_t<ServiceMethodType&&> {
        m_DatabaseConnection.BeginTransaction();
        try {
            if constexpr (std::is_void_v<std::invoke_result_t<ServiceMethodType&&>>) {
                std::invoke(std::forward<ServiceMethodType>(ServiceMethod));
                m_DatabaseConnection.Commit();
            } else if constexpr (IsStdExpectedType<std::remove_cvref_t<std::invoke_result_t<ServiceMethodType&&>>>::value) {
                std::invoke_result_t<ServiceMethodType&&> Result{ std::invoke(std::forward<ServiceMethodType>(ServiceMethod)) };
                if (Result.has_value()) {
                    m_DatabaseConnection.Commit();
                } else {
                    m_DatabaseConnection.Rollback();
                }
                return Result;
            } else {
                static_assert(AlwaysFalseType<std::invoke_result_t<ServiceMethodType&&>>::value,
                              "TransactionWrapper only supports void or std::expected return types");
            }
        } catch (...) {
            m_DatabaseConnection.Rollback();
            throw;
        }
    }
};

}

#pragma once

#include <duckdb.hpp>

#include <expected>
#include <functional>
#include <type_traits>
#include <utility>

namespace Infrastructure::Sql {

class TransactionRunner final {
private:
    template <typename>
    struct IsStdExpectedType : std::false_type {};

    template <typename SuccessType, typename ErrorType>
    struct IsStdExpectedType<std::expected<SuccessType, ErrorType>> : std::true_type {};

    template <typename>
    struct AlwaysFalseType : std::false_type {};

public:
    explicit TransactionRunner(duckdb::Connection& DatabaseConnection) noexcept
        : m_DatabaseConnection{ DatabaseConnection } {
    }

    ~TransactionRunner() = default;
    TransactionRunner(const TransactionRunner&) = delete;
    TransactionRunner(TransactionRunner&&) = delete;
    TransactionRunner& operator=(const TransactionRunner&) = delete;
    TransactionRunner& operator=(TransactionRunner&&) = delete;

    template <typename ServiceMethodType>
    [[nodiscard]] std::invoke_result_t<ServiceMethodType&&> TransactionWrapper(ServiceMethodType&& ServiceMethod) {
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

private:
    duckdb::Connection& m_DatabaseConnection;
};

}

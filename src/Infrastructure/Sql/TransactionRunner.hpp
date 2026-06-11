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

    template <typename Fn>
    [[nodiscard]] std::invoke_result_t<Fn&&> TransactionWrapper(Fn&& Function) {
        m_DatabaseConnection.BeginTransaction();
        try {
            if constexpr (std::is_void_v<std::invoke_result_t<Fn&&>>) {
                std::invoke(std::forward<Fn>(Function));
                m_DatabaseConnection.Commit();
            } else if constexpr (IsStdExpectedType<std::remove_cvref_t<std::invoke_result_t<Fn&&>>>::value) {
                std::invoke_result_t<Fn&&> Result{ std::invoke(std::forward<Fn>(Function)) };
                if (Result.has_value()) {
                    m_DatabaseConnection.Commit();
                } else {
                    m_DatabaseConnection.Rollback();
                }
                return Result;
            } else {
                static_assert(AlwaysFalseType<std::invoke_result_t<Fn&&>>::value, "TransactionWrapper only supports void or std::expected return types");
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

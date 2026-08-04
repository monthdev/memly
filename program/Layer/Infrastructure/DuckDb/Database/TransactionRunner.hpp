#pragma once

#include <duckdb.hpp>

#include <concepts>
#include <exception>
#include <functional>
#include <initializer_list>
#include <source_location>
#include <string_view>
#include <type_traits>
#include <utility>

#include "Support/Runtime/Exception/ThrowMemlyException.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

class TransactionRunner final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    duckdb::Connection& m_DatabaseConnection;

public:
    explicit TransactionRunner(duckdb::Connection& DatabaseConnection) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DatabaseConnection{ DatabaseConnection } {
    }

    template <typename LambdaType>
        requires std::invocable<LambdaType&&>
    [[nodiscard]] auto TransactionWrapper(LambdaType&& Lambda, const std::source_location& SourceLocation = std::source_location::current())
        -> std::invoke_result_t<LambdaType&&> {
        this->m_DatabaseConnection.BeginTransaction();
        try {
            if constexpr (std::same_as<std::invoke_result_t<LambdaType&&>, void>) {
                std::invoke(std::forward<LambdaType>(Lambda));
                this->m_DatabaseConnection.Commit();
            } else {
                std::invoke_result_t<LambdaType&&> Result{ std::invoke(std::forward<LambdaType>(Lambda)) };
                this->m_DatabaseConnection.Commit();
                return std::invoke_result_t<LambdaType&&>{ std::move(Result) };
            }
        } catch (const std::exception& TransactionException) {
            try {
                this->m_DatabaseConnection.Rollback();
            } catch (const std::exception& RollbackException) {
                Support::Runtime::Exception::ThrowMemlyException(
                    std::initializer_list<std::string_view>{
                        "Transaction failed:\n\t", TransactionException.what(), "\nRollback also failed:\n\t", RollbackException.what() },
                    SourceLocation);
            }
            throw;
        } catch (...) {
            try {
                this->m_DatabaseConnection.Rollback();
            } catch (const std::exception& RollbackException) {
                Support::Runtime::Exception::ThrowMemlyException(
                    std::initializer_list<std::string_view>{ "Transaction failed with a non-standard exception\nRollback also failed:\n\t",
                                                             RollbackException.what() },
                    SourceLocation);
            }
            throw;
        }
    }
};

}

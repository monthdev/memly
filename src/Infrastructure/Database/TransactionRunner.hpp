#pragma once

#include <duckdb.hpp>

#include <concepts>
#include <expected>
#include <functional>
#include <type_traits>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Database {

class TransactionRunner final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    template <typename CandidateType>
    struct IsStdExpectedType : std::false_type, private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
        explicit constexpr IsStdExpectedType() noexcept
            : std::false_type{}
            , Support::SpecialMemberPolicy::NoCopyNoMoveMixin{} {
        }
    };

    template <typename SuccessType, typename ErrorType>
    struct IsStdExpectedType<std::expected<SuccessType, ErrorType>> : std::true_type, private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
        explicit constexpr IsStdExpectedType() noexcept
            : std::true_type{}
            , Support::SpecialMemberPolicy::NoCopyNoMoveMixin{} {
        }
    };

    template <typename DependentType>
    struct AlwaysFalseType : std::false_type, private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
        explicit constexpr AlwaysFalseType() noexcept
            : std::false_type{}
            , Support::SpecialMemberPolicy::NoCopyNoMoveMixin{} {
        }
    };

    duckdb::Connection& m_DatabaseConnection;

public:
    explicit TransactionRunner(duckdb::Connection& DatabaseConnection) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DatabaseConnection{ DatabaseConnection } {
    }

    template <typename ServiceMethodType>
        requires std::invocable<ServiceMethodType&&>
    [[nodiscard]] auto TransactionWrapper(ServiceMethodType&& ServiceMethod) -> std::invoke_result_t<ServiceMethodType&&> {
        m_DatabaseConnection.BeginTransaction();
        try {
            if constexpr (std::is_void_v<std::invoke_result_t<ServiceMethodType&&>>) {
                std::invoke(std::forward<ServiceMethodType>(ServiceMethod));
                m_DatabaseConnection.Commit();
            } else if constexpr (IsStdExpectedType<std::remove_cvref_t<std::invoke_result_t<ServiceMethodType&&>>>::value) {
                std::invoke_result_t<ServiceMethodType&&> ResultExpected{ std::invoke(std::forward<ServiceMethodType>(ServiceMethod)) };
                if (ResultExpected.has_value()) {
                    m_DatabaseConnection.Commit();
                } else {
                    m_DatabaseConnection.Rollback();
                }
                return ResultExpected;
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

#pragma once

#include <array>
#include <cstddef>
#include <exception>
#include <initializer_list>
#include <source_location>
#include <string_view>

#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Support::Runtime::Exception {
class [[nodiscard]] MemlyException final : public std::exception, private SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    static constexpr std::size_t s_ErrorMessageCapacity{ 1024 };

    std::array<char, s_ErrorMessageCapacity + 1> m_ErrorMessageArray;
    std::size_t m_ErrorMessageSize;

public:
    explicit MemlyException(const std::initializer_list<std::string_view> ErrorMessageInitializerList, const std::source_location& SourceLocation) noexcept
        : std::exception{}
        , SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_ErrorMessageArray{}
        , m_ErrorMessageSize{} {
        this->ConstructErrorMessage(ErrorMessageInitializerList, SourceLocation);
    }

    [[nodiscard]] auto what() const noexcept -> const char* override;

private:
    void ConstructErrorMessage(std::initializer_list<std::string_view>, const std::source_location&) noexcept;
    void AppendErrorMessage(std::string_view) noexcept;
};
}

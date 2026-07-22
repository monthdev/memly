#pragma once

#include <array>
#include <cstddef>
#include <exception>
#include <source_location>
#include <string_view>

#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Support::Runtime {
class [[nodiscard]] MemlyException final : public std::exception, private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    static constexpr std::size_t s_ErrorMessageCapacity{ 1024 };

    std::array<char, s_ErrorMessageCapacity + 1> m_ErrorMessageArray;
    std::size_t m_ErrorMessageSize;

public:
    explicit MemlyException(const std::string_view ErrorMessage, const std::source_location& SourceLocation) noexcept
        : std::exception{}
        , Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_ErrorMessageArray{}
        , m_ErrorMessageSize{} {
        ConstructErrorMessage(ErrorMessage, SourceLocation);
    }

    [[nodiscard]] auto what() const noexcept -> const char* override;

private:
    void ConstructErrorMessage(std::string_view, const std::source_location&) noexcept;
    void AppendErrorMessage(std::string_view) noexcept;
};
}

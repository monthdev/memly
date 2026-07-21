#pragma once

#include <array>
#include <cstddef>
#include <exception>
#include <source_location>
#include <string_view>

namespace Support::Runtime {
class [[nodiscard]] MemlyException final : public std::exception {
private:
    inline static constexpr std::size_t s_ErrorMessageCapacity{ 1024 };

    std::array<char, s_ErrorMessageCapacity + 1> m_ErrorMessageArray;
    std::size_t m_ErrorMessageSize;

public:
    explicit MemlyException(const std::string_view ErrorMessage, const std::source_location& SourceLocation) noexcept
        : std::exception{}
        , m_ErrorMessageArray{}
        , m_ErrorMessageSize{} {
        ConstructErrorMessage(ErrorMessage, SourceLocation);
    }

    explicit MemlyException(const MemlyException&) = delete;
    explicit MemlyException(MemlyException&&) = delete;
    auto operator=(const MemlyException&) -> MemlyException& = delete;
    auto operator=(MemlyException&&) -> MemlyException& = delete;

    [[nodiscard]] auto what() const noexcept -> const char* override;

private:
    void ConstructErrorMessage(const std::string_view, const std::source_location&) noexcept;
    void AppendErrorMessage(const std::string_view) noexcept;
};
}

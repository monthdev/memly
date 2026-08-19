#pragma once

#include <array>
#include <cstddef>
#include <exception>
#include <initializer_list>
#include <source_location>
#include <string_view>

namespace Memly::Exception {
class [[nodiscard]] MemlyException final : public std::exception {
private:
    static constexpr std::size_t s_ErrorMessageCapacity{ 1024 };

    std::array<char, s_ErrorMessageCapacity + 1> m_ErrorMessageArray;
    std::size_t m_ErrorMessageSize;

public:
    explicit MemlyException(std::initializer_list<std::string_view>, std::source_location = std::source_location::current()) noexcept;

    explicit MemlyException(const MemlyException&) = delete;
    auto operator=(const MemlyException&) -> MemlyException& = delete;

    explicit MemlyException(MemlyException&&) = delete;
    auto operator=(MemlyException&&) -> MemlyException& = delete;

    ~MemlyException() noexcept override = default;

    [[nodiscard]] auto what() const noexcept -> const char* override;

private:
    void ConstructErrorMessage(std::initializer_list<std::string_view>, std::source_location) noexcept;
    void AppendErrorMessage(std::string_view) noexcept;
};
}

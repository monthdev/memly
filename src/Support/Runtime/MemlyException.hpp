#pragma once

#include <array>
#include <cstddef>
#include <exception>
#include <source_location>
#include <string_view>

namespace Support::Runtime {
class [[nodiscard]] MemlyException final : public std::exception {
public:
    explicit MemlyException(const std::string_view ErrorMessage, const std::source_location& SourceLocation) noexcept
        : m_ErrorMessageArray{}
        , m_ErrorMessageSize{ 0 } {
        ConstructErrorMessage(ErrorMessage, SourceLocation);
    }

    MemlyException(const MemlyException&) = delete;
    MemlyException(MemlyException&&) = delete;
    MemlyException& operator=(const MemlyException&) = delete;
    MemlyException& operator=(MemlyException&&) = delete;

    [[nodiscard]] const char* what() const noexcept override {
        return m_ErrorMessageArray.data();
    }

private:
    void ConstructErrorMessage(const std::string_view, const std::source_location&) noexcept;
    void AppendErrorMessage(const std::string_view) noexcept;

    inline static constexpr std::size_t s_ErrorMessageCapacity{ 1024 };

    std::array<char, s_ErrorMessageCapacity + 1> m_ErrorMessageArray;
    std::size_t m_ErrorMessageSize;
};
}

#include "Memly/Exception/MemlyException.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <cstddef>
#include <exception>
#include <initializer_list>
#include <limits>
#include <source_location>
#include <span>
#include <string_view>
#include <system_error>

namespace Support::Exception {

MemlyException::MemlyException(const std::initializer_list<std::string_view>& ErrorMessageInitializerList, const std::source_location& SourceLocation) noexcept
    : std::exception{}
    , m_ErrorMessageArray{}
    , m_ErrorMessageSize{} {
    this->ConstructErrorMessage(ErrorMessageInitializerList, SourceLocation);
}

[[nodiscard]] auto MemlyException::what() const noexcept -> const char* {
    return this->m_ErrorMessageArray.data();
}

void MemlyException::ConstructErrorMessage(const std::initializer_list<std::string_view> ErrorMessageInitializerList,
                                           const std::source_location& SourceLocation) noexcept {
    this->AppendErrorMessage("Exception thrown in ");
    this->AppendErrorMessage(SourceLocation.file_name());
    this->AppendErrorMessage(", ");
    this->AppendErrorMessage(SourceLocation.function_name());
    this->AppendErrorMessage(", line ");
    std::array<char, std::numeric_limits<decltype(SourceLocation.line())>::digits10 + 1> SourceLocationLineArray{};
    const std::to_chars_result SourceLocationLineToCharsResult{ std::to_chars(
        SourceLocationLineArray.begin(), SourceLocationLineArray.end(), SourceLocation.line()) };
    assert(SourceLocationLineToCharsResult.ec == std::errc{});
    this->AppendErrorMessage(
        std::string_view{ SourceLocationLineArray.data(), static_cast<std::size_t>(SourceLocationLineToCharsResult.ptr - SourceLocationLineArray.data()) });
    this->AppendErrorMessage(":\n\t");
    for (const std::string_view ErrorMessage : ErrorMessageInitializerList) { this->AppendErrorMessage(ErrorMessage); }
}

void MemlyException::AppendErrorMessage(const std::string_view CharsToAppend) noexcept {
    const std::size_t AvailableCopySize{ std::min(CharsToAppend.size(), this->m_ErrorMessageArray.size() - 1 - this->m_ErrorMessageSize) };
    std::ranges::copy(std::span{ CharsToAppend }.first(AvailableCopySize),
                      std::span{ this->m_ErrorMessageArray }.subspan(this->m_ErrorMessageSize, AvailableCopySize).begin());
    this->m_ErrorMessageSize += AvailableCopySize;
}
}

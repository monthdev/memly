#include "Support/Runtime/MemlyException.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <cstddef>
#include <limits>
#include <source_location>
#include <span>
#include <string_view>
#include <system_error>

namespace Support::Runtime {

[[nodiscard]] auto MemlyException::what() const noexcept -> const char* {
    return m_ErrorMessageArray.data();
}

void MemlyException::ConstructErrorMessage(const std::string_view ErrorMessage, const std::source_location& SourceLocation) noexcept {
    AppendErrorMessage("Exception thrown in ");
    AppendErrorMessage(SourceLocation.file_name());
    AppendErrorMessage(", ");
    AppendErrorMessage(SourceLocation.function_name());
    AppendErrorMessage(", line ");
    std::array<char, std::numeric_limits<decltype(SourceLocation.line())>::digits10 + 1> SourceLocationLineArray{};
    const std::to_chars_result SourceLocationLineToCharsResult{ std::to_chars(
        SourceLocationLineArray.begin(), SourceLocationLineArray.end(), SourceLocation.line()) };
    assert(SourceLocationLineToCharsResult.ec == std::errc{});
    AppendErrorMessage(
        std::string_view{ SourceLocationLineArray.data(), static_cast<std::size_t>(SourceLocationLineToCharsResult.ptr - SourceLocationLineArray.data()) });
    AppendErrorMessage(":\n\t");
    AppendErrorMessage(ErrorMessage);
}

void MemlyException::AppendErrorMessage(const std::string_view CharsToAppend) noexcept {
    const std::size_t AvailableCopySize{ std::min(CharsToAppend.size(), m_ErrorMessageArray.size() - 1 - m_ErrorMessageSize) };
    std::ranges::copy(std::span{ CharsToAppend }.first(AvailableCopySize),
                      std::span{ m_ErrorMessageArray }.subspan(m_ErrorMessageSize, AvailableCopySize).begin());
    m_ErrorMessageSize += AvailableCopySize;
}
}

#include "Application/Domain/Text/Data/HumanTextInput.hpp"

#include <unicode/brkiter.h>
#include <unicode/locid.h>
#include <unicode/normalizer2.h>
#include <unicode/unistr.h>
#include <unicode/utypes.h>

#include <cstddef>
#include <memory>
#include <string>

#include "Support/Runtime/ThrowMemlyException.hpp"

namespace Application::Domain::Text::Data {
namespace {

void a_ThrowOnIcuError(const UErrorCode ErrorCode) {
    if (U_FAILURE(ErrorCode) not_eq 0) {
        Support::Runtime::ThrowMemlyException(u_errorName(ErrorCode));
    }
}

[[nodiscard]] auto a_NormalizeText(const icu::UnicodeString& UnicodeString) -> icu::UnicodeString {
    UErrorCode ErrorCode{ U_ZERO_ERROR };
    const icu::Normalizer2* NormalizerPointer{ icu::Normalizer2::getNFCInstance(ErrorCode) };
    a_ThrowOnIcuError(ErrorCode);
    icu::UnicodeString NormalizedUnicodeString{};
    NormalizerPointer->normalize(UnicodeString, NormalizedUnicodeString, ErrorCode);
    a_ThrowOnIcuError(ErrorCode);
    return NormalizedUnicodeString;
}

}

[[nodiscard]] auto HumanTextInput::FromInput(const std::string& Text) -> HumanTextInput {
    return HumanTextInput{ a_NormalizeText(icu::UnicodeString::fromUTF8(Text)) };
}

[[nodiscard]] auto HumanTextInput::ComputeGraphemeClusterLength() const -> std::size_t {
    thread_local std::unique_ptr<icu::BreakIterator> s_BreakIteratorUniquePointer{ []() -> std::unique_ptr<icu::BreakIterator> {
        UErrorCode ErrorCode{ U_ZERO_ERROR };
        std::unique_ptr<icu::BreakIterator> BreakIteratorUniquePointer{ icu::BreakIterator::createCharacterInstance(icu::Locale::getRoot(), ErrorCode) };
        a_ThrowOnIcuError(ErrorCode);
        return BreakIteratorUniquePointer;
    }() };
    icu::BreakIterator& BreakIterator{ *s_BreakIteratorUniquePointer };
    BreakIterator.setText(m_NormalizedUnicodeString);
    std::size_t GraphemeClusterLength{ 0 };
    BreakIterator.first();
    while (BreakIterator.next() not_eq icu::BreakIterator::DONE) { ++GraphemeClusterLength; }
    return GraphemeClusterLength;
}

[[nodiscard]] auto HumanTextInput::ToNormalizedStdString() const -> std::string {
    std::string Text{};
    m_NormalizedUnicodeString.toUTF8String(Text);
    return Text;
}

[[nodiscard]] auto HumanTextInput::ToNormalizedCaseFoldedStdString() && -> std::string {
    std::string Text{};
    m_NormalizedUnicodeString.foldCase().toUTF8String(Text);
    return Text;
}

}

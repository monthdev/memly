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

[[nodiscard]] auto a_GetThreadLocalIcuBreakIterator() -> icu::BreakIterator& {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif
    thread_local std::unique_ptr<icu::BreakIterator> s_IcuBreakIteratorUniquePointer{ []() -> std::unique_ptr<icu::BreakIterator> {
        UErrorCode ErrorCode{ U_ZERO_ERROR };
        std::unique_ptr<icu::BreakIterator> IcuBreakIteratorUniquePointer{ icu::BreakIterator::createCharacterInstance(icu::Locale::getRoot(), ErrorCode) };
        a_ThrowOnIcuError(ErrorCode);
        return IcuBreakIteratorUniquePointer;
    }() };
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    return *s_IcuBreakIteratorUniquePointer;
}

}

[[nodiscard]] auto HumanTextInput::FromInput(const std::string& Text) -> HumanTextInput {
    return HumanTextInput{ a_NormalizeText(icu::UnicodeString::fromUTF8(Text)) };
}

[[nodiscard]] auto HumanTextInput::ComputeGraphemeClusterLength() const -> std::size_t {
    icu::BreakIterator& IcuBreakIterator{ a_GetThreadLocalIcuBreakIterator() };
    IcuBreakIterator.setText(m_NormalizedUnicodeString);
    std::size_t GraphemeClusterLength{ 0 };
    IcuBreakIterator.first();
    while (IcuBreakIterator.next() not_eq icu::BreakIterator::DONE) { ++GraphemeClusterLength; }
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

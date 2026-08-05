#include "Layer/Application/Domain/Text/Data/HumanTextInput.hpp"

#include <unicode/brkiter.h>
#include <unicode/errorcode.h>
#include <unicode/locid.h>
#include <unicode/normalizer2.h>
#include <unicode/unistr.h>

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "Support/Runtime/Exception/ThrowMemlyException.hpp"

namespace Layer::Application::Domain::Text::Data {
namespace {

void u_ThrowOnIcuError(const icu::ErrorCode& IcuErrorCode) {
    if (IcuErrorCode.isFailure() not_eq 0) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ IcuErrorCode.errorName() });
    }
}

[[nodiscard]] auto u_NormalizeText(const icu::UnicodeString& UnicodeString) -> icu::UnicodeString {
    icu::ErrorCode IcuErrorCode{};
    const icu::Normalizer2* NormalizerPointer{ icu::Normalizer2::getNFCInstance(IcuErrorCode) };
    u_ThrowOnIcuError(IcuErrorCode);
    icu::UnicodeString NormalizedUnicodeString{};
    NormalizerPointer->normalize(UnicodeString, NormalizedUnicodeString, IcuErrorCode);
    u_ThrowOnIcuError(IcuErrorCode);
    return icu::UnicodeString{ std::move(NormalizedUnicodeString) };
}

}

[[nodiscard]] auto HumanTextInput::FromInput(const std::string& Text) -> HumanTextInput {
    return HumanTextInput{ u_NormalizeText(icu::UnicodeString::fromUTF8(Text)) };
}

namespace {

[[nodiscard]] auto u_GetThreadLocalIcuBreakIterator() -> icu::BreakIterator& {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif
    thread_local std::unique_ptr<icu::BreakIterator> s_IcuBreakIteratorUniquePointer{ std::invoke([]() -> std::unique_ptr<icu::BreakIterator> {
        icu::ErrorCode IcuErrorCode{};
        std::unique_ptr<icu::BreakIterator> IcuBreakIteratorUniquePointer{ icu::BreakIterator::createCharacterInstance(icu::Locale::getRoot(), IcuErrorCode) };
        u_ThrowOnIcuError(IcuErrorCode);
        return std::unique_ptr<icu::BreakIterator>{ std::move(IcuBreakIteratorUniquePointer) };
    }) };
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    return *s_IcuBreakIteratorUniquePointer;
}

}

[[nodiscard]] auto HumanTextInput::ComputeGraphemeClusterLength() const -> std::size_t {
    icu::BreakIterator& IcuBreakIterator{ u_GetThreadLocalIcuBreakIterator() };
    IcuBreakIterator.setText(this->m_NormalizedUnicodeString);
    std::size_t GraphemeClusterLength{ 0 };
    IcuBreakIterator.first();
    while (IcuBreakIterator.next() not_eq icu::BreakIterator::DONE) { ++GraphemeClusterLength; }
    return std::size_t{ GraphemeClusterLength };
}

[[nodiscard]] auto HumanTextInput::ToNormalizedStdString() const -> std::string {
    std::string Text{};
    this->m_NormalizedUnicodeString.toUTF8String(Text);
    return std::string{ std::move(Text) };
}

[[nodiscard]] auto HumanTextInput::ToNormalizedCaseFoldedStdString() && -> std::string {
    std::string Text{};
    this->m_NormalizedUnicodeString.foldCase().toUTF8String(Text);
    return std::string{ std::move(Text) };
}

}

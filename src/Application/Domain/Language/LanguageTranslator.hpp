#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Application::Domain::Language {
enum class [[nodiscard]] TargetLanguage : std::uint8_t {
    NoLanguage,
    Afrikaans,
    Amharic,
    Arabic,
    Bulgarian,
    Bengali,
    Bosnian,
    Catalan,
    Czech,
    Welsh,
    Danish,
    German,
    Greek,
    English,
    Estonian,
    Basque,
    Finnish,
    French,
    Galician,
    Gujarati,
    Hausa,
    Hindi,
    Croatian,
    Hungarian,
    Indonesian,
    Icelandic,
    Italian,
    Hebrew,
    Japanese,
    Javanese,
    Khmer,
    Kannada,
    Korean,
    Latin,
    Lithuanian,
    Latvian,
    Malayalam,
    Marathi,
    Malay,
    MyanmarBurmese,
    Nepali,
    Dutch,
    Norwegian,
    Punjabi,
    Polish,
    PortugueseBrazil,
    PortuguesePortugal,
    Romanian,
    Russian,
    Sinhala,
    Slovak,
    Albanian,
    Serbian,
    Sundanese,
    Swedish,
    Swahili,
    Tamil,
    Telugu,
    Thai,
    Filipino,
    Turkish,
    Ukrainian,
    Urdu,
    Vietnamese,
    Cantonese,
    ChineseSimplified,
    ChineseTraditional,
    Count
};

struct TargetLanguageInfo : private Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin {
    TargetLanguage m_Language;
    std::string_view m_GoogleTranslateCode;
    std::string_view m_GoogleTextToSpeechLanguageCode;
    std::string_view m_GoogleTextToSpeechTopLevelDomain;
    std::string_view m_DisplayName; // TODO: Should struct really hold the gui display name?

    explicit constexpr TargetLanguageInfo(const TargetLanguage Language,
                                          const std::string_view GoogleTranslateCode,
                                          const std::string_view GoogleTextToSpeechLanguageCode,
                                          const std::string_view GoogleTextToSpeechTopLevelDomain,
                                          const std::string_view DisplayName) noexcept
        : Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin{}
        , m_Language{ Language }
        , m_GoogleTranslateCode{ GoogleTranslateCode }
        , m_GoogleTextToSpeechLanguageCode{ GoogleTextToSpeechLanguageCode }
        , m_GoogleTextToSpeechTopLevelDomain{ GoogleTextToSpeechTopLevelDomain }
        , m_DisplayName{ DisplayName } {
    }
};

[[nodiscard]] auto GetSupportedTargetLanguages() noexcept -> const std::array<TargetLanguageInfo, std::to_underlying(TargetLanguage::Count)>&;
[[nodiscard]] auto GetTargetLanguageInfo(TargetLanguage) -> const TargetLanguageInfo&;
}

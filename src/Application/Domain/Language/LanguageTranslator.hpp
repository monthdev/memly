#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace Application::Domain::Language {
enum class TargetLanguage : std::uint8_t {
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
    ChineseTraditional
};

struct TargetLanguageInfo {
    TargetLanguage m_Language;
    std::string_view m_GoogleTranslateCode;
    std::string_view m_GoogleTtsLangCode;
    std::string_view m_GoogleTtsTld;
    std::string_view m_GuiName;
};

[[nodiscard]] const std::array<TargetLanguageInfo, 67>& GetSupportedTargetLanguages() noexcept;
[[nodiscard]] const TargetLanguageInfo& GetTargetLanguageInfo(TargetLanguage);
}

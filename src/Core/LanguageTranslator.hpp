#pragma once

#include <cstdint>
#include <span>
#include <string_view>

namespace Core {
enum class TargetLanguage : std::uint8_t {
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
    TargetLanguage Language;
    std::string_view GoogleTranslateCode;
    std::string_view GoogleTtsLangCode;
    std::string_view GoogleTtsTld;
    std::string_view GuiName;
};

std::span<const TargetLanguageInfo> GetSupportedTargetLanguages();
const TargetLanguageInfo& GetTargetLanguageInfo(TargetLanguage Language);
}

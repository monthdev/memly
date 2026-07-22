#include "Application/Domain/Language/LanguageTranslator.hpp"

#include <array>
#include <cstddef>
#include <utility>

namespace Application::Domain::Language {
[[nodiscard]] auto GetSupportedTargetLanguages() noexcept -> const std::array<TargetLanguageInfo, std::to_underlying(TargetLanguageEnum::Count)>& {
    // TODO: Move array to unnamed namespace?
    static constexpr std::array<TargetLanguageInfo, std::to_underlying(TargetLanguageEnum::Count)> s_TargetLanguages{
        {
         TargetLanguageInfo{ TargetLanguageEnum::NoLanguage, "", "", "", "(No Language)" },
         TargetLanguageInfo{ TargetLanguageEnum::Afrikaans, "af", "af", "", "Afrikaans" },
         TargetLanguageInfo{ TargetLanguageEnum::Amharic, "am", "am", "", "Amharic" },
         TargetLanguageInfo{ TargetLanguageEnum::Arabic, "ar", "ar", "", "Arabic" },
         TargetLanguageInfo{ TargetLanguageEnum::Bulgarian, "bg", "bg", "", "Bulgarian" },
         TargetLanguageInfo{ TargetLanguageEnum::Bengali, "bn", "bn", "", "Bengali" },
         TargetLanguageInfo{ TargetLanguageEnum::Bosnian, "bs", "bs", "", "Bosnian" },
         TargetLanguageInfo{ TargetLanguageEnum::Catalan, "ca", "ca", "", "Catalan" },
         TargetLanguageInfo{ TargetLanguageEnum::Czech, "cs", "cs", "", "Czech" },
         TargetLanguageInfo{ TargetLanguageEnum::Welsh, "cy", "cy", "", "Welsh" },
         TargetLanguageInfo{ TargetLanguageEnum::Danish, "da", "da", "", "Danish" },
         TargetLanguageInfo{ TargetLanguageEnum::German, "de", "de", "", "German" },
         TargetLanguageInfo{ TargetLanguageEnum::Greek, "el", "el", "", "Greek" },
         TargetLanguageInfo{ TargetLanguageEnum::English, "en", "en", "", "English" },
         TargetLanguageInfo{ TargetLanguageEnum::Estonian, "et", "et", "", "Estonian" },
         TargetLanguageInfo{ TargetLanguageEnum::Basque, "eu", "eu", "", "Basque" },
         TargetLanguageInfo{ TargetLanguageEnum::Finnish, "fi", "fi", "", "Finnish" },
         TargetLanguageInfo{ TargetLanguageEnum::French, "fr", "fr", "fr", "French" },
         TargetLanguageInfo{ TargetLanguageEnum::Galician, "gl", "gl", "", "Galician" },
         TargetLanguageInfo{ TargetLanguageEnum::Gujarati, "gu", "gu", "", "Gujarati" },
         TargetLanguageInfo{ TargetLanguageEnum::Hausa, "ha", "ha", "", "Hausa" },
         TargetLanguageInfo{ TargetLanguageEnum::Hindi, "hi", "hi", "", "Hindi" },
         TargetLanguageInfo{ TargetLanguageEnum::Croatian, "hr", "hr", "", "Croatian" },
         TargetLanguageInfo{ TargetLanguageEnum::Hungarian, "hu", "hu", "", "Hungarian" },
         TargetLanguageInfo{ TargetLanguageEnum::Indonesian, "id", "id", "", "Indonesian" },
         TargetLanguageInfo{ TargetLanguageEnum::Icelandic, "is", "is", "", "Icelandic" },
         TargetLanguageInfo{ TargetLanguageEnum::Italian, "it", "it", "", "Italian" },
         TargetLanguageInfo{ TargetLanguageEnum::Hebrew, "iw", "iw", "", "Hebrew" },
         TargetLanguageInfo{ TargetLanguageEnum::Japanese, "ja", "ja", "", "Japanese" },
         TargetLanguageInfo{ TargetLanguageEnum::Javanese, "jw", "jw", "", "Javanese" },
         TargetLanguageInfo{ TargetLanguageEnum::Khmer, "km", "km", "", "Khmer" },
         TargetLanguageInfo{ TargetLanguageEnum::Kannada, "kn", "kn", "", "Kannada" },
         TargetLanguageInfo{ TargetLanguageEnum::Korean, "ko", "ko", "", "Korean" },
         TargetLanguageInfo{ TargetLanguageEnum::Latin, "la", "la", "", "Latin" },
         TargetLanguageInfo{ TargetLanguageEnum::Lithuanian, "lt", "lt", "", "Lithuanian" },
         TargetLanguageInfo{ TargetLanguageEnum::Latvian, "lv", "lv", "", "Latvian" },
         TargetLanguageInfo{ TargetLanguageEnum::Malayalam, "ml", "ml", "", "Malayalam" },
         TargetLanguageInfo{ TargetLanguageEnum::Marathi, "mr", "mr", "", "Marathi" },
         TargetLanguageInfo{ TargetLanguageEnum::Malay, "ms", "ms", "", "Malay" },
         TargetLanguageInfo{ TargetLanguageEnum::MyanmarBurmese, "my", "my", "", "Myanmar (Burmese)" },
         TargetLanguageInfo{ TargetLanguageEnum::Nepali, "ne", "ne", "", "Nepali" },
         TargetLanguageInfo{ TargetLanguageEnum::Dutch, "nl", "nl", "", "Dutch" },
         TargetLanguageInfo{ TargetLanguageEnum::Norwegian, "no", "no", "", "Norwegian" },
         TargetLanguageInfo{ TargetLanguageEnum::Punjabi, "pa", "pa", "", "Punjabi" },
         TargetLanguageInfo{ TargetLanguageEnum::Polish, "pl", "pl", "", "Polish" },
         TargetLanguageInfo{ TargetLanguageEnum::PortugueseBrazil, "pt", "pt", "com.br", "Portuguese (Brazil)" },
         TargetLanguageInfo{ TargetLanguageEnum::PortuguesePortugal, "por", "pt", "pt", "Portuguese (Portugal)" },
         TargetLanguageInfo{ TargetLanguageEnum::Romanian, "ro", "ro", "", "Romanian" },
         TargetLanguageInfo{ TargetLanguageEnum::Russian, "ru", "ru", "", "Russian" },
         TargetLanguageInfo{ TargetLanguageEnum::Sinhala, "si", "si", "", "Sinhala" },
         TargetLanguageInfo{ TargetLanguageEnum::Slovak, "sk", "sk", "", "Slovak" },
         TargetLanguageInfo{ TargetLanguageEnum::Albanian, "sq", "sq", "", "Albanian" },
         TargetLanguageInfo{ TargetLanguageEnum::Serbian, "sr", "sr", "", "Serbian" },
         TargetLanguageInfo{ TargetLanguageEnum::Sundanese, "su", "su", "", "Sundanese" },
         TargetLanguageInfo{ TargetLanguageEnum::Swedish, "sv", "sv", "", "Swedish" },
         TargetLanguageInfo{ TargetLanguageEnum::Swahili, "sw", "sw", "", "Swahili" },
         TargetLanguageInfo{ TargetLanguageEnum::Tamil, "ta", "ta", "", "Tamil" },
         TargetLanguageInfo{ TargetLanguageEnum::Telugu, "te", "te", "", "Telugu" },
         TargetLanguageInfo{ TargetLanguageEnum::Thai, "th", "th", "", "Thai" },
         TargetLanguageInfo{ TargetLanguageEnum::Filipino, "tl", "tl", "", "Filipino" },
         TargetLanguageInfo{ TargetLanguageEnum::Turkish, "tr", "tr", "", "Turkish" },
         TargetLanguageInfo{ TargetLanguageEnum::Ukrainian, "uk", "uk", "", "Ukrainian" },
         TargetLanguageInfo{ TargetLanguageEnum::Urdu, "ur", "ur", "", "Urdu" },
         TargetLanguageInfo{ TargetLanguageEnum::Vietnamese, "vi", "vi", "", "Vietnamese" },
         TargetLanguageInfo{ TargetLanguageEnum::Cantonese, "yue", "yue", "", "Cantonese" },
         TargetLanguageInfo{ TargetLanguageEnum::ChineseSimplified, "zh-cn", "zh-CN", "", "Chinese (Simplified)" },
         TargetLanguageInfo{ TargetLanguageEnum::ChineseTraditional, "zh-tw", "zh-TW", "", "Chinese (Traditional)" },
         }
    };
    return s_TargetLanguages;
}

[[nodiscard]] auto GetTargetLanguageInfo(const TargetLanguageEnum Language) -> const TargetLanguageInfo& {
    return GetSupportedTargetLanguages().at(static_cast<std::size_t>(Language));
}
}

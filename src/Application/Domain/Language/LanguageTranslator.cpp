#include "Application/Domain/Language/LanguageTranslator.hpp"

#include <array>
#include <cstddef>
#include <utility>

namespace Application::Domain::Language {
[[nodiscard]] auto GetSupportedTargetLanguages() noexcept -> const std::array<TargetLanguageInfo, std::to_underlying(TargetLanguage::Count)>& {
    // TODO: Move array to unnamed namespace?
    static constexpr std::array<TargetLanguageInfo, std::to_underlying(TargetLanguage::Count)> s_TargetLanguages{
        {
         TargetLanguageInfo{ TargetLanguage::NoLanguage, "", "", "", "(No Language)" },
         TargetLanguageInfo{ TargetLanguage::Afrikaans, "af", "af", "", "Afrikaans" },
         TargetLanguageInfo{ TargetLanguage::Amharic, "am", "am", "", "Amharic" },
         TargetLanguageInfo{ TargetLanguage::Arabic, "ar", "ar", "", "Arabic" },
         TargetLanguageInfo{ TargetLanguage::Bulgarian, "bg", "bg", "", "Bulgarian" },
         TargetLanguageInfo{ TargetLanguage::Bengali, "bn", "bn", "", "Bengali" },
         TargetLanguageInfo{ TargetLanguage::Bosnian, "bs", "bs", "", "Bosnian" },
         TargetLanguageInfo{ TargetLanguage::Catalan, "ca", "ca", "", "Catalan" },
         TargetLanguageInfo{ TargetLanguage::Czech, "cs", "cs", "", "Czech" },
         TargetLanguageInfo{ TargetLanguage::Welsh, "cy", "cy", "", "Welsh" },
         TargetLanguageInfo{ TargetLanguage::Danish, "da", "da", "", "Danish" },
         TargetLanguageInfo{ TargetLanguage::German, "de", "de", "", "German" },
         TargetLanguageInfo{ TargetLanguage::Greek, "el", "el", "", "Greek" },
         TargetLanguageInfo{ TargetLanguage::English, "en", "en", "", "English" },
         TargetLanguageInfo{ TargetLanguage::Estonian, "et", "et", "", "Estonian" },
         TargetLanguageInfo{ TargetLanguage::Basque, "eu", "eu", "", "Basque" },
         TargetLanguageInfo{ TargetLanguage::Finnish, "fi", "fi", "", "Finnish" },
         TargetLanguageInfo{ TargetLanguage::French, "fr", "fr", "fr", "French" },
         TargetLanguageInfo{ TargetLanguage::Galician, "gl", "gl", "", "Galician" },
         TargetLanguageInfo{ TargetLanguage::Gujarati, "gu", "gu", "", "Gujarati" },
         TargetLanguageInfo{ TargetLanguage::Hausa, "ha", "ha", "", "Hausa" },
         TargetLanguageInfo{ TargetLanguage::Hindi, "hi", "hi", "", "Hindi" },
         TargetLanguageInfo{ TargetLanguage::Croatian, "hr", "hr", "", "Croatian" },
         TargetLanguageInfo{ TargetLanguage::Hungarian, "hu", "hu", "", "Hungarian" },
         TargetLanguageInfo{ TargetLanguage::Indonesian, "id", "id", "", "Indonesian" },
         TargetLanguageInfo{ TargetLanguage::Icelandic, "is", "is", "", "Icelandic" },
         TargetLanguageInfo{ TargetLanguage::Italian, "it", "it", "", "Italian" },
         TargetLanguageInfo{ TargetLanguage::Hebrew, "iw", "iw", "", "Hebrew" },
         TargetLanguageInfo{ TargetLanguage::Japanese, "ja", "ja", "", "Japanese" },
         TargetLanguageInfo{ TargetLanguage::Javanese, "jw", "jw", "", "Javanese" },
         TargetLanguageInfo{ TargetLanguage::Khmer, "km", "km", "", "Khmer" },
         TargetLanguageInfo{ TargetLanguage::Kannada, "kn", "kn", "", "Kannada" },
         TargetLanguageInfo{ TargetLanguage::Korean, "ko", "ko", "", "Korean" },
         TargetLanguageInfo{ TargetLanguage::Latin, "la", "la", "", "Latin" },
         TargetLanguageInfo{ TargetLanguage::Lithuanian, "lt", "lt", "", "Lithuanian" },
         TargetLanguageInfo{ TargetLanguage::Latvian, "lv", "lv", "", "Latvian" },
         TargetLanguageInfo{ TargetLanguage::Malayalam, "ml", "ml", "", "Malayalam" },
         TargetLanguageInfo{ TargetLanguage::Marathi, "mr", "mr", "", "Marathi" },
         TargetLanguageInfo{ TargetLanguage::Malay, "ms", "ms", "", "Malay" },
         TargetLanguageInfo{ TargetLanguage::MyanmarBurmese, "my", "my", "", "Myanmar (Burmese)" },
         TargetLanguageInfo{ TargetLanguage::Nepali, "ne", "ne", "", "Nepali" },
         TargetLanguageInfo{ TargetLanguage::Dutch, "nl", "nl", "", "Dutch" },
         TargetLanguageInfo{ TargetLanguage::Norwegian, "no", "no", "", "Norwegian" },
         TargetLanguageInfo{ TargetLanguage::Punjabi, "pa", "pa", "", "Punjabi" },
         TargetLanguageInfo{ TargetLanguage::Polish, "pl", "pl", "", "Polish" },
         TargetLanguageInfo{ TargetLanguage::PortugueseBrazil, "pt", "pt", "com.br", "Portuguese (Brazil)" },
         TargetLanguageInfo{ TargetLanguage::PortuguesePortugal, "por", "pt", "pt", "Portuguese (Portugal)" },
         TargetLanguageInfo{ TargetLanguage::Romanian, "ro", "ro", "", "Romanian" },
         TargetLanguageInfo{ TargetLanguage::Russian, "ru", "ru", "", "Russian" },
         TargetLanguageInfo{ TargetLanguage::Sinhala, "si", "si", "", "Sinhala" },
         TargetLanguageInfo{ TargetLanguage::Slovak, "sk", "sk", "", "Slovak" },
         TargetLanguageInfo{ TargetLanguage::Albanian, "sq", "sq", "", "Albanian" },
         TargetLanguageInfo{ TargetLanguage::Serbian, "sr", "sr", "", "Serbian" },
         TargetLanguageInfo{ TargetLanguage::Sundanese, "su", "su", "", "Sundanese" },
         TargetLanguageInfo{ TargetLanguage::Swedish, "sv", "sv", "", "Swedish" },
         TargetLanguageInfo{ TargetLanguage::Swahili, "sw", "sw", "", "Swahili" },
         TargetLanguageInfo{ TargetLanguage::Tamil, "ta", "ta", "", "Tamil" },
         TargetLanguageInfo{ TargetLanguage::Telugu, "te", "te", "", "Telugu" },
         TargetLanguageInfo{ TargetLanguage::Thai, "th", "th", "", "Thai" },
         TargetLanguageInfo{ TargetLanguage::Filipino, "tl", "tl", "", "Filipino" },
         TargetLanguageInfo{ TargetLanguage::Turkish, "tr", "tr", "", "Turkish" },
         TargetLanguageInfo{ TargetLanguage::Ukrainian, "uk", "uk", "", "Ukrainian" },
         TargetLanguageInfo{ TargetLanguage::Urdu, "ur", "ur", "", "Urdu" },
         TargetLanguageInfo{ TargetLanguage::Vietnamese, "vi", "vi", "", "Vietnamese" },
         TargetLanguageInfo{ TargetLanguage::Cantonese, "yue", "yue", "", "Cantonese" },
         TargetLanguageInfo{ TargetLanguage::ChineseSimplified, "zh-cn", "zh-CN", "", "Chinese (Simplified)" },
         TargetLanguageInfo{ TargetLanguage::ChineseTraditional, "zh-tw", "zh-TW", "", "Chinese (Traditional)" },
         }
    };
    return s_TargetLanguages;
}

[[nodiscard]] auto GetTargetLanguageInfo(const TargetLanguage Language) -> const TargetLanguageInfo& {
    return GetSupportedTargetLanguages().at(static_cast<std::size_t>(Language));
}
}

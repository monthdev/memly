#include "LanguageTranslator.hpp"

#include <array>
#include <cstddef>

namespace Core {
std::span<const TargetLanguageInfo> GetSupportedTargetLanguages() {
    static constexpr std::array<TargetLanguageInfo, 67> TargetLanguages{
        {
         { TargetLanguage::NoLanguage, "", "", "", "(No Language)" },
         { TargetLanguage::Afrikaans, "af", "af", "", "Afrikaans" },
         { TargetLanguage::Amharic, "am", "am", "", "Amharic" },
         { TargetLanguage::Arabic, "ar", "ar", "", "Arabic" },
         { TargetLanguage::Bulgarian, "bg", "bg", "", "Bulgarian" },
         { TargetLanguage::Bengali, "bn", "bn", "", "Bengali" },
         { TargetLanguage::Bosnian, "bs", "bs", "", "Bosnian" },
         { TargetLanguage::Catalan, "ca", "ca", "", "Catalan" },
         { TargetLanguage::Czech, "cs", "cs", "", "Czech" },
         { TargetLanguage::Welsh, "cy", "cy", "", "Welsh" },
         { TargetLanguage::Danish, "da", "da", "", "Danish" },
         { TargetLanguage::German, "de", "de", "", "German" },
         { TargetLanguage::Greek, "el", "el", "", "Greek" },
         { TargetLanguage::English, "en", "en", "", "English" },
         { TargetLanguage::Estonian, "et", "et", "", "Estonian" },
         { TargetLanguage::Basque, "eu", "eu", "", "Basque" },
         { TargetLanguage::Finnish, "fi", "fi", "", "Finnish" },
         { TargetLanguage::French, "fr", "fr", "fr", "French" },
         { TargetLanguage::Galician, "gl", "gl", "", "Galician" },
         { TargetLanguage::Gujarati, "gu", "gu", "", "Gujarati" },
         { TargetLanguage::Hausa, "ha", "ha", "", "Hausa" },
         { TargetLanguage::Hindi, "hi", "hi", "", "Hindi" },
         { TargetLanguage::Croatian, "hr", "hr", "", "Croatian" },
         { TargetLanguage::Hungarian, "hu", "hu", "", "Hungarian" },
         { TargetLanguage::Indonesian, "id", "id", "", "Indonesian" },
         { TargetLanguage::Icelandic, "is", "is", "", "Icelandic" },
         { TargetLanguage::Italian, "it", "it", "", "Italian" },
         { TargetLanguage::Hebrew, "iw", "iw", "", "Hebrew" },
         { TargetLanguage::Japanese, "ja", "ja", "", "Japanese" },
         { TargetLanguage::Javanese, "jw", "jw", "", "Javanese" },
         { TargetLanguage::Khmer, "km", "km", "", "Khmer" },
         { TargetLanguage::Kannada, "kn", "kn", "", "Kannada" },
         { TargetLanguage::Korean, "ko", "ko", "", "Korean" },
         { TargetLanguage::Latin, "la", "la", "", "Latin" },
         { TargetLanguage::Lithuanian, "lt", "lt", "", "Lithuanian" },
         { TargetLanguage::Latvian, "lv", "lv", "", "Latvian" },
         { TargetLanguage::Malayalam, "ml", "ml", "", "Malayalam" },
         { TargetLanguage::Marathi, "mr", "mr", "", "Marathi" },
         { TargetLanguage::Malay, "ms", "ms", "", "Malay" },
         { TargetLanguage::MyanmarBurmese, "my", "my", "", "Myanmar (Burmese)" },
         { TargetLanguage::Nepali, "ne", "ne", "", "Nepali" },
         { TargetLanguage::Dutch, "nl", "nl", "", "Dutch" },
         { TargetLanguage::Norwegian, "no", "no", "", "Norwegian" },
         { TargetLanguage::Punjabi, "pa", "pa", "", "Punjabi" },
         { TargetLanguage::Polish, "pl", "pl", "", "Polish" },
         { TargetLanguage::PortugueseBrazil, "pt", "pt", "com.br", "Portuguese (Brazil)" },
         { TargetLanguage::PortuguesePortugal, "por", "pt", "pt", "Portuguese (Portugal)" },
         { TargetLanguage::Romanian, "ro", "ro", "", "Romanian" },
         { TargetLanguage::Russian, "ru", "ru", "", "Russian" },
         { TargetLanguage::Sinhala, "si", "si", "", "Sinhala" },
         { TargetLanguage::Slovak, "sk", "sk", "", "Slovak" },
         { TargetLanguage::Albanian, "sq", "sq", "", "Albanian" },
         { TargetLanguage::Serbian, "sr", "sr", "", "Serbian" },
         { TargetLanguage::Sundanese, "su", "su", "", "Sundanese" },
         { TargetLanguage::Swedish, "sv", "sv", "", "Swedish" },
         { TargetLanguage::Swahili, "sw", "sw", "", "Swahili" },
         { TargetLanguage::Tamil, "ta", "ta", "", "Tamil" },
         { TargetLanguage::Telugu, "te", "te", "", "Telugu" },
         { TargetLanguage::Thai, "th", "th", "", "Thai" },
         { TargetLanguage::Filipino, "tl", "tl", "", "Filipino" },
         { TargetLanguage::Turkish, "tr", "tr", "", "Turkish" },
         { TargetLanguage::Ukrainian, "uk", "uk", "", "Ukrainian" },
         { TargetLanguage::Urdu, "ur", "ur", "", "Urdu" },
         { TargetLanguage::Vietnamese, "vi", "vi", "", "Vietnamese" },
         { TargetLanguage::Cantonese, "yue", "yue", "", "Cantonese" },
         { TargetLanguage::ChineseSimplified, "zh-cn", "zh-CN", "", "Chinese (Simplified)" },
         { TargetLanguage::ChineseTraditional, "zh-tw", "zh-TW", "", "Chinese (Traditional)" },
         }
    };
    static_assert(TargetLanguages.size() == 67);
    return TargetLanguages;
}

const TargetLanguageInfo& GetTargetLanguageInfo(TargetLanguage Language) {
    return GetSupportedTargetLanguages()[static_cast<std::size_t>(Language)];
}
}

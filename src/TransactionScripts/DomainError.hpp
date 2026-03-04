#pragma once

#include <expected>
#include <vector>

template <typename FieldType>
    requires std::is_enum_v<FieldType>
struct DomainError {
    FieldType Field;
    std::string_view Message;

    DomainError(FieldType Field, std::string_view Message)
        : Field{ Field }
        , Message{ Message } {
    }
};

template <typename FieldType>
using DomainErrorVector = std::vector<DomainError<FieldType>>;

template <typename FieldType>
using ValidationResult = std::expected<void, DomainErrorVector<FieldType>>;

// template <typename FieldType>
// inline std::string ValidationResultToString(const ValidationResult<FieldType>& Res) {
//     std::string Out;

//     const DomainErrorVector<FieldType>& errors = Res.error();
//     for (std::size_t I = 0; I < errors.size(); I++) {
//         Out.append(errors.at(I).Message);
//         if (I < errors.size() - 1)
//             Out.append(", ");
//     }
//     return Out;
// }

#pragma once

#include <string>
#include <utility>

#include "Layer/Infrastructure/DuckDb/Database/DecodableQueryResultRowMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Repository {

struct [[nodiscard]] MutatedId final : public Database::DecodableQueryResultRowMixin<std::string> {
    std::string m_MutatedId;

    explicit MutatedId(std::string&& MutatedId) noexcept
        : DecodableQueryResultRowMixin{}
        , m_MutatedId{ std::move(MutatedId) } {
    }
};

}

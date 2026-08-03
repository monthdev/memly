#pragma once

#include <string>
#include <utility>

#include "Layer/Infrastructure/Database/DecodableQueryResultRowMixin.hpp"

namespace Layer::Infrastructure::Store {

struct [[nodiscard]] MutatedId final : public Database::DecodableQueryResultRowMixin<std::string> {
    std::string m_MutatedId;

    explicit MutatedId(std::string&& MutatedId)
        : DecodableQueryResultRowMixin{}
        , m_MutatedId{ std::move(MutatedId) } {
    }
};

}

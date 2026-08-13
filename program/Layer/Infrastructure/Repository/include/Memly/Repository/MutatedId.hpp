#pragma once

#include <string>
#include <utility>

#include "Memly/Database/DecodableQueryResultRowMixin.hpp"

namespace Layer::Infrastructure::Repository {

struct [[nodiscard]] MutatedId final : public Database::DecodableQueryResultRowMixin<std::string> {
    std::string m_MutatedId;

    explicit MutatedId(std::string&& MutatedId) noexcept
        : DecodableQueryResultRowMixin{}
        , m_MutatedId{ std::move(MutatedId) } {
    }
};

}

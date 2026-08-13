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

    explicit MutatedId(const MutatedId&) = delete;
    auto operator=(const MutatedId&) -> MutatedId& = delete;

    explicit MutatedId(MutatedId&&) noexcept = default;
    auto operator=(MutatedId&&) -> MutatedId& = delete;

    ~MutatedId() noexcept = default;
};

}

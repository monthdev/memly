module;

#include <string>

export module Memly.Repository.MutatedId;

import Memly.Database.DecodableQueryResultRowMixin;

export namespace Memly::Repository {

struct [[nodiscard]] MutatedId final : public Database::DecodableQueryResultRowMixin<std::string> {
    std::string m_MutatedId;

    explicit MutatedId(std::string&&) noexcept;

    explicit MutatedId(const MutatedId&) = delete;
    auto operator=(const MutatedId&) -> MutatedId& = delete;

    explicit MutatedId(MutatedId&&) noexcept = default;
    auto operator=(MutatedId&&) -> MutatedId& = delete;

    ~MutatedId() noexcept = default;
};

}

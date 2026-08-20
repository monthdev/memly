module;

#include <string>
#include <utility>

module Memly.Repository;

import Memly.Database;

namespace Memly::Repository {

MutatedId::MutatedId(std::string&& MutatedId) noexcept
    : DecodableQueryResultRowMixin{}
    , m_MutatedId{ std::move(MutatedId) } {
}

}

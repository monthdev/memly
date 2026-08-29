module;

#include <string>
#include <utility>

module Memly.Repository.MutatedId;

import Memly.Database.DecodableQueryResultRowMixin;

namespace Memly::Repository {

MutatedId::MutatedId(std::string&& MutatedId) noexcept :
    DecodableQueryResultRowMixin{}, m_MutatedId{ std::move(MutatedId) } {
}

}

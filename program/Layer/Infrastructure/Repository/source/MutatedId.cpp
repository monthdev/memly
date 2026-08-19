#include "Memly/Repository/MutatedId.hpp"

#include <string>
#include <utility>

#include "Memly/Database/DecodableQueryResultRowMixin.hpp"

namespace Memly::Repository {

MutatedId::MutatedId(std::string&& MutatedId) noexcept
    : DecodableQueryResultRowMixin{}
    , m_MutatedId{ std::move(MutatedId) } {
}

}

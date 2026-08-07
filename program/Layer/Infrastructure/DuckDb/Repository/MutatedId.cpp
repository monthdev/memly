#include "Layer/Infrastructure/DuckDb/Repository/MutatedId.hpp"

#include <string>
#include <utility>

#include "Layer/Infrastructure/DuckDb/Database/DecodableQueryResultRowMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Repository {

MutatedId::MutatedId(std::string&& MutatedId)
    : DecodableQueryResultRowMixin{}
    , m_MutatedId{ std::move(MutatedId) } {
}

}

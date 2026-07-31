// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <cstdint>
#include <optional>

#include "Infrastructure/Database/DatabaseRuntime.hpp"
#include "Infrastructure/Database/PreparedStatement.hpp"
#include "Infrastructure/Sql/Library/Query/LibraryQuerySql.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Store::Library {

class LibraryClockStore final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Database::DatabaseRuntime& m_DatabaseRuntime;
    Database::PreparedStatement m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement;

public:
    explicit LibraryClockStore(Database::DatabaseRuntime& DatabaseRuntime)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DatabaseRuntime{ DatabaseRuntime }
        , m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement{
            DatabaseRuntime.PrepareStatement(Sql::Library::Query::ReadNextLibraryInvalidationAtMillisecondsSinceEpochSql())
        } {
    }

    [[nodiscard]] auto ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(std::int64_t) -> std::optional<std::int64_t>;
};

}
#endif

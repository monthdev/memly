// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <cstdint>
#include <optional>

#include "Layer/Infrastructure/DuckDb/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/DuckDb/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/DuckDb/Repository/Library/Sql/LibrarySql.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Repository::Library {

class LibraryRepository final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Database::DatabaseRuntime& m_DatabaseRuntime;
    Database::PreparedStatement m_SelectNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement;

public:
    explicit LibraryRepository(Database::DatabaseRuntime& DatabaseRuntime)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DatabaseRuntime{ DatabaseRuntime }
        , m_SelectNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement{
            DatabaseRuntime.PrepareStatement(Sql::SelectNextLibraryInvalidationAtMillisecondsSinceEpochSql())
        } {
    }

    [[nodiscard]] auto ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(std::int64_t) -> std::optional<std::int64_t>;
};

}
#endif

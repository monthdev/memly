// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <cstdint>
#include <optional>

#include "Memly/Database/DatabaseRuntime.hpp"
#include "Memly/Database/PreparedStatement.hpp"
#include "Memly/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Repository {

class LibraryRepository final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Database::DatabaseRuntime& m_DatabaseRuntime;
    Database::PreparedStatement m_SelectNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement;

public:
    explicit LibraryRepository(Database::DatabaseRuntime&);

    [[nodiscard]] auto ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(std::int64_t) -> std::optional<std::int64_t>;
};

}
#endif

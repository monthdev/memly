export module Memly.Repository.LibraryRepository;

// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <cstdint>
#include <optional>

import Memly.Database.DatabaseRuntime;
import Memly.Database.PreparedStatement;

export namespace Memly::Repository {

class LibraryRepository final {
private:
    Database::DatabaseRuntime& m_DatabaseRuntime;
    Database::PreparedStatement m_SelectNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement;

public:
    explicit LibraryRepository(Database::DatabaseRuntime&);

    explicit LibraryRepository(const LibraryRepository&) = delete;
    auto operator=(const LibraryRepository&) -> LibraryRepository& = delete;

    explicit LibraryRepository(LibraryRepository&&) = delete;
    auto operator=(LibraryRepository&&) -> LibraryRepository& = delete;

    ~LibraryRepository() noexcept = default;

    [[nodiscard]] auto ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(std::int64_t) -> std::optional<std::int64_t>;
};

}
#endif

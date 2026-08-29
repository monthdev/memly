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
    Database::PreparedStatement
        m_SelectNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement;

public:
    explicit LibraryRepository(Database::DatabaseRuntime&);

    explicit LibraryRepository(const LibraryRepository&) = delete;
    LibraryRepository&
    operator=(const LibraryRepository&) = delete;

    explicit LibraryRepository(LibraryRepository&&) = delete;
    LibraryRepository&
    operator=(LibraryRepository&&) = delete;

    ~LibraryRepository() noexcept = default;

    [[nodiscard]] std::optional<std::int64_t>
    ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(std::int64_t);
};

}
#endif

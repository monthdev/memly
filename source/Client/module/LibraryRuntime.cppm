/// \file
/// \brief Declares the owned library runtime object graph.

module;

#include <string>

export module Memly.Client.LibraryRuntime;

import Memly.Database.DatabaseRuntime;
import Memly.Repository.DeckRepository;
import Memly.Service.DeckService;

export namespace Memly::Client {

/// \attention Keep `LibraryRuntime` members ordered from lower-level
/// dependencies to higher-level dependents.
class LibraryRuntime final {
private:
    Database::DatabaseRuntime m_DatabaseRuntime;
    // Invalidation::LibraryInvalidationChannel m_LibraryInvalidationChannel;
    // Repository::LibraryRepository m_LibraryRepository;
    // Invalidation::LibraryInvalidationCoordinator
    // m_LibraryInvalidationCoordinator;
    Repository::DeckRepository m_DeckRepository;
    // Repository::ReviewSessionRepository m_ReviewSessionRepository;
    Service::DeckService m_DeckService;
    // Service::ReviewSessionListService m_ReviewSessionListService;
    // Service::ReviewSessionService m_ReviewSessionService;

public:
    explicit LibraryRuntime(const std::string&);

    explicit LibraryRuntime(const LibraryRuntime&) = delete;
    LibraryRuntime&
    operator=(const LibraryRuntime&) = delete;

    explicit LibraryRuntime(LibraryRuntime&&) = delete;
    LibraryRuntime&
    operator=(LibraryRuntime&&) = delete;

    ~LibraryRuntime() noexcept = default;
};

}

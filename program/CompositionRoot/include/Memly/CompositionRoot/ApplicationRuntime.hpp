/// \file
/// \brief Declares the owned application runtime object graph.

#pragma once

#include <string>

#include "Memly/Database/DatabaseRuntime.hpp"
#include "Memly/Repository/DeckRepository.hpp"
#include "Memly/Service/DeckService.hpp"

namespace Memly::CompositionRoot {

/// \attention Keep `ApplicationRuntime` members ordered from lower-level dependencies to higher-level dependents.
class ApplicationRuntime final {
private:
    Database::DatabaseRuntime m_DatabaseRuntime;
    // Invalidation::LibraryInvalidationChannel m_LibraryInvalidationChannel;
    // Repository::LibraryRepository m_LibraryRepository;
    // Invalidation::LibraryInvalidationCoordinator m_LibraryInvalidationCoordinator;
    Repository::DeckRepository m_DeckRepository;
    // Repository::ReviewSessionRepository m_ReviewSessionRepository;
    Service::DeckService m_DeckService;
    // Service::ReviewSessionListService m_ReviewSessionListService;
    // Service::ReviewSessionService m_ReviewSessionService;

public:
    explicit ApplicationRuntime(const std::string&);

    explicit ApplicationRuntime(const ApplicationRuntime&) = delete;
    auto operator=(const ApplicationRuntime&) -> ApplicationRuntime& = delete;

    explicit ApplicationRuntime(ApplicationRuntime&&) = delete;
    auto operator=(ApplicationRuntime&&) -> ApplicationRuntime& = delete;

    ~ApplicationRuntime() noexcept = default;
};

}

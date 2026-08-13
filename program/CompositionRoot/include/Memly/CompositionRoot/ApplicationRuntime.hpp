/// \file
/// \brief Declares the owned application runtime object graph.

#pragma once

#include <string>

#include "Memly/Database/DatabaseRuntime.hpp"
#include "Memly/Repository/DeckRepository.hpp"
#include "Memly/Service/DeckService.hpp"

namespace CompositionRoot {

/// \attention Keep `ApplicationRuntime` members ordered from lower-level dependencies to higher-level dependents.
class ApplicationRuntime final {
private:
    Layer::Infrastructure::Database::DatabaseRuntime m_DatabaseRuntime;
    // Layer::Application::Invalidation::LibraryInvalidationChannel m_LibraryInvalidationChannel;
    // Layer::Infrastructure::Repository::LibraryRepository m_LibraryRepository;
    // Layer::Application::Invalidation::LibraryInvalidationCoordinator m_LibraryInvalidationCoordinator;
    Layer::Infrastructure::Repository::DeckRepository m_DeckRepository;
    // Layer::Infrastructure::Repository::ReviewSessionRepository m_ReviewSessionRepository;
    Layer::Application::Service::DeckService m_DeckService;
    // Layer::Application::Service::ReviewSessionListService m_ReviewSessionListService;
    // Layer::Application::Service::ReviewSessionService m_ReviewSessionService;

public:
    explicit ApplicationRuntime(const std::string&);

    explicit ApplicationRuntime(const ApplicationRuntime&) = delete;
    auto operator=(const ApplicationRuntime&) -> ApplicationRuntime& = delete;

    explicit ApplicationRuntime(ApplicationRuntime&&) = delete;
    auto operator=(ApplicationRuntime&&) -> ApplicationRuntime& = delete;

    ~ApplicationRuntime() noexcept = default;
};

}

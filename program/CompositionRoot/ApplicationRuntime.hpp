/// \file
/// \brief Declares the owned application runtime object graph.
///
/// \attention Keep `ApplicationRuntime` members ordered from lower-level dependencies to higher-level dependents.

#pragma once

#include <string>

#include "Layer/Application/Service/Deck/DeckService.hpp"
#include "Layer/Infrastructure/DuckDb/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/DuckDb/Repository/Deck/DeckRepository.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace CompositionRoot {

class ApplicationRuntime final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Layer::Infrastructure::DuckDb::Database::DatabaseRuntime m_DatabaseRuntime;
    // Layer::Application::Invalidation::LibraryInvalidationChannel m_LibraryInvalidationChannel;
    // Layer::Infrastructure::DuckDb::Repository::Library::LibraryRepository m_LibraryRepository;
    // Layer::Application::Invalidation::LibraryInvalidationCoordinator m_LibraryInvalidationCoordinator;
    Layer::Infrastructure::DuckDb::Repository::Deck::DeckRepository m_DeckRepository;
    // Layer::Infrastructure::DuckDb::Repository::ReviewSession::ReviewSessionRepository m_ReviewSessionRepository;
    Layer::Application::Service::Deck::DeckService m_DeckService;
    // Layer::Application::Service::ReviewSession::ReviewSessionListService m_ReviewSessionListService;
    // Layer::Application::Service::ReviewSession::ReviewSessionService m_ReviewSessionService;

public:
    explicit ApplicationRuntime(const std::string&);
};

}

/// \file
/// \brief Defines construction of the owned application runtime object graph.

#include "CompositionRoot/ApplicationRuntime.hpp"

#include <string>

#include "Layer/Infrastructure/DuckDb/Database/DatabaseMigrator.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace CompositionRoot {

ApplicationRuntime::ApplicationRuntime(const std::string& DatabaseFilePath)
    : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
    , m_DatabaseRuntime{ Layer::Infrastructure::DuckDb::Database::DatabaseMigrator{ DatabaseFilePath }.ApplyMigrations() }
    // , m_LibraryInvalidationChannel{}
    // , m_LibraryRepository{ m_DatabaseRuntime }
    // , m_LibraryInvalidationCoordinator{ m_LibraryInvalidationChannel, m_LibraryRepository }
    , m_DeckRepository{ m_DatabaseRuntime } // , m_ReviewSessionRepository{ m_DatabaseRuntime }
    , m_DeckService{ m_DeckRepository } {
    // Reintroduce ReviewSessionService after the application transaction boundary is defined.
}

}

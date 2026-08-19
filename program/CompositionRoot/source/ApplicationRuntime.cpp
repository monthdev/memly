/// \file
/// \brief Defines construction of the owned application runtime object graph.

#include "Memly/CompositionRoot/ApplicationRuntime.hpp"

#include <string>

#include "Memly/Database/DatabaseMigrator.hpp"

namespace Memly::CompositionRoot {

ApplicationRuntime::ApplicationRuntime(const std::string& DatabaseFilePath)
    : m_DatabaseRuntime{ Database::DatabaseMigrator{ DatabaseFilePath }.ApplyMigrations() } // , m_LibraryInvalidationChannel{}
                                                                                            // , m_LibraryRepository{ m_DatabaseRuntime }
                                                                                            // , m_LibraryInvalidationCoordinator{
                                                                                            // m_LibraryInvalidationChannel,
                                                                                            // m_LibraryRepository }
    , m_DeckRepository{ this->m_DatabaseRuntime } // , m_ReviewSessionRepository{ m_DatabaseRuntime }
    , m_DeckService{ this->m_DeckRepository } {
    // Reintroduce ReviewSessionService after the application transaction boundary is defined.
}

}

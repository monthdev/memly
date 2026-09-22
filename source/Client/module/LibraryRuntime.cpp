/// \file
/// \brief Defines construction of the owned library runtime object graph.

module;

#include <string>

module Memly.Client.LibraryRuntime;

import Memly.Database.DatabaseMigrator;

namespace Memly::Client {

LibraryRuntime::LibraryRuntime(const std::string& DatabaseFilePath) :
    m_DatabaseRuntime{
        Database::DatabaseMigrator{ DatabaseFilePath }.ApplyMigrations(),
    } // , m_LibraryInvalidationChannel{}
      // , m_LibraryRepository{ m_DatabaseRuntime }
      // , m_LibraryInvalidationCoordinator{
      // m_LibraryInvalidationChannel,
      // m_LibraryRepository }
    ,
    m_DeckRepository{ this->m_DatabaseRuntime } // ,
                                                // m_ReviewSessionRepository{
                                                // m_DatabaseRuntime }
    ,
    m_DeckService{ this->m_DeckRepository } {
    // Reintroduce ReviewSessionService after the application transaction
    // boundary is defined.
}

}

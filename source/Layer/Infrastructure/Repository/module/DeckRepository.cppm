module;

#include <cstdint>
#include <string>
#include <vector>

export module Memly.Repository.DeckRepository;

import Memly.Database.DatabaseRuntime;
import Memly.Database.PreparedStatement;
import Memly.Repository.DeckSnapshotRecord;

export namespace Memly::Repository {

class DeckRepository final {
private:
    Database::PreparedStatement m_SelectDeckSnapshotRecordsPreparedStatement;
    Database::PreparedStatement m_InsertRootDeckPreparedStatement;
    Database::PreparedStatement m_InsertChildDeckPreparedStatement;
    Database::PreparedStatement m_UpdateDeckParentToRootPreparedStatement;
    Database::PreparedStatement m_UpdateDeckParentPreparedStatement;
    Database::PreparedStatement m_UpdateDeckNamePreparedStatement;
    Database::PreparedStatement m_DeleteDeckCardReviewsPreparedStatement;
    Database::PreparedStatement m_DeleteDeckCardsPreparedStatement;
    Database::PreparedStatement m_DeleteDeckPreparedStatement;

public:
    explicit DeckRepository(Database::DatabaseRuntime&);

    explicit DeckRepository(const DeckRepository&) = delete;
    DeckRepository&
    operator=(const DeckRepository&) = delete;

    explicit DeckRepository(DeckRepository&&) = delete;
    DeckRepository&
    operator=(DeckRepository&&) = delete;

    ~DeckRepository() noexcept = default;

    [[nodiscard]] std::vector<DeckSnapshotRecord>
    ReadDeckSnapshotRecords(std::int64_t);

    void
    CreateRootDeck(const std::string&, std::uint8_t);

    void
    CreateChildDeck(const std::string&, const std::string&);

    void
    MoveDeckToRoot(const std::string&);

    void
    MoveDeckUnderParent(const std::string&, const std::string&);

    void
    RenameDeck(const std::string&, const std::string&);

    void
    DeleteDeck(const std::string&);
};

}

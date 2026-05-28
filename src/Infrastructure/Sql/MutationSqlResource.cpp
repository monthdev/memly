#include "MutationSqlResource.hpp"

#include "SqlResourceBytes.hpp"

namespace Infrastructure::Sql {

[[nodiscard]] std::string CreateRootDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/CreateRootDeck.sql");
}

[[nodiscard]] std::string CreateChildDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/CreateChildDeck.sql");
}

[[nodiscard]] std::string MoveDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/MoveDeck.sql");
}

[[nodiscard]] std::string RenameDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/RenameDeck.sql");
}

[[nodiscard]] std::string CreateCustomReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/CreateCustomReviewSession.sql");
}

[[nodiscard]] std::string CreateDefaultReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/CreateDefaultReviewSession.sql");
}

[[nodiscard]] std::string CreateCustomReviewSessionDeckSelectionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/CreateCustomReviewSessionDeckSelection.sql");
}

[[nodiscard]] std::string RenameReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/RenameReviewSession.sql");
}

[[nodiscard]] std::string UpdateReviewSessionToDefaultSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/UpdateReviewSessionToDefault.sql");
}

[[nodiscard]] std::string UpdateReviewSessionToCustomSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/UpdateReviewSessionToCustom.sql");
}

[[nodiscard]] std::string UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpochSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/UpdateReviewSessionLastCardReviewAtMillisecondsSinceEpoch.sql");
}

[[nodiscard]] std::string DeleteDeckCardReviewsSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/DeleteDeckCardReviews.sql");
}

[[nodiscard]] std::string DeleteDeckCardsSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/DeleteDeckCards.sql");
}

[[nodiscard]] std::string DeleteDeckSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/DeleteDeck.sql");
}

[[nodiscard]] std::string DeleteCustomReviewSessionDeckSelectionsSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/DeleteCustomReviewSessionDeckSelections.sql");
}

[[nodiscard]] std::string DeleteReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/DeleteReviewSession.sql");
}
}

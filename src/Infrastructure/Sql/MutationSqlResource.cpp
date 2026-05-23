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

[[nodiscard]] std::string CreateReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/CreateReviewSession.sql");
}

[[nodiscard]] std::string CreateDefaultReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/CreateDefaultReviewSession.sql");
}

[[nodiscard]] std::string CreateReviewSessionDeckSelectionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/CreateReviewSessionDeckSelection.sql");
}

[[nodiscard]] std::string CreateDefaultReviewSessionDeckBindingSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/CreateDefaultReviewSessionDeckBinding.sql");
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

[[nodiscard]] std::string DeleteDefaultReviewSessionDeckBindingSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/DeleteDefaultReviewSessionDeckBinding.sql");
}

[[nodiscard]] std::string DeleteReviewSessionDeckSelectionsSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/DeleteReviewSessionDeckSelections.sql");
}

[[nodiscard]] std::string DeleteReviewSessionSql() {
    return ReadSqlResourceBytes(":/Sql/Mutation/DeleteReviewSession.sql");
}
}

module;

#include <string>
#include <string_view>

export module Memly.Domain.HumanText;

export namespace Memly::Domain {

class [[nodiscard]] HumanText final {
private:
    std::string m_NormalizedText;
    std::string m_NormalizedCaseFoldedText;

    explicit HumanText(std::string&&, std::string&&) noexcept;

public:
    explicit HumanText(const HumanText&) = delete;
    auto operator=(const HumanText&) -> HumanText& = delete;

    explicit HumanText(HumanText&&) noexcept = default;
    auto operator=(HumanText&&) -> HumanText& = delete;

    ~HumanText() noexcept = default;

    [[nodiscard]] static auto FromPersisted(std::string&&, std::string&&) noexcept -> HumanText;

    [[nodiscard]] auto GetNormalizedStdStringView() const& noexcept [[clang::lifetimebound]] -> std::string_view;
    [[nodiscard]] auto GetNormalizedStdStringView() const&& noexcept -> std::string_view = delete;

    [[nodiscard]] auto GetNormalizedCaseFoldedStdStringView() const& noexcept [[clang::lifetimebound]] -> std::string_view;
    [[nodiscard]] auto GetNormalizedCaseFoldedStdStringView() const&& noexcept -> std::string_view = delete;
};

}

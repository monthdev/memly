export module Memly.Invalidation.LibraryInvalidationChannel;

// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>

import Memly.Invalidation.LibraryInvalidationTarget;

export namespace Memly::Invalidation {

class LibraryInvalidationChannel final : public QObject {
    Q_OBJECT

private:
    std::int64_t m_CurrentSnapshotAsOfMillisecondsSinceEpoch;

public:
    explicit LibraryInvalidationChannel(QObject* = nullptr);

    explicit LibraryInvalidationChannel(
        const LibraryInvalidationChannel&
    ) = delete;
    LibraryInvalidationChannel&
    operator=(const LibraryInvalidationChannel&) = delete;

    explicit LibraryInvalidationChannel(LibraryInvalidationChannel&&) = delete;
    LibraryInvalidationChannel&
    operator=(LibraryInvalidationChannel&&) = delete;

    ~LibraryInvalidationChannel() noexcept override = default;

    [[nodiscard]] std::int64_t
    GetCurrentSnapshotAsOfMillisecondsSinceEpoch() const noexcept;

    void
    SetCurrentSnapshotAsOfMillisecondsSinceEpoch(std::int64_t) noexcept;

    void
    PublishInvalidation(const LibraryInvalidationTargetBitset&) noexcept;

    template <typename ControllerType, typename ControllerRefreshMethodType>
        requires std::is_member_function_pointer_v<
                     ControllerRefreshMethodType> and
                 std::is_nothrow_invocable_v<
                     ControllerRefreshMethodType,
                     ControllerType*>
    void
    Connect(
        ControllerType* ControllerPointer,
        const LibraryInvalidationTargetEnum ControllerLibraryInvalidationTarget,
        const ControllerRefreshMethodType ControllerRefreshMethod
    ) {
        this->ConnectInvalidationSignal(
            ControllerPointer,
            ControllerLibraryInvalidationTarget,
            [ControllerPointer, ControllerRefreshMethod]() noexcept -> void {
                std::invoke(ControllerRefreshMethod, ControllerPointer);
            }
        );
    }

    template <typename ControllerType, typename ControllerRefreshMethodType>
        requires std::is_member_function_pointer_v<
                     ControllerRefreshMethodType> and
                 std::is_nothrow_invocable_v<
                     ControllerRefreshMethodType,
                     ControllerType*,
                     std::int64_t>
    void
    ConnectSnapshot(
        ControllerType* ControllerPointer,
        const LibraryInvalidationTargetEnum ControllerLibraryInvalidationTarget,
        const ControllerRefreshMethodType ControllerRefreshMethod
    ) {
        this->ConnectInvalidationSignal(
            ControllerPointer,
            ControllerLibraryInvalidationTarget,
            [this, ControllerPointer, ControllerRefreshMethod]() noexcept
                -> void {
                std::invoke(
                    ControllerRefreshMethod,
                    ControllerPointer,
                    this->m_CurrentSnapshotAsOfMillisecondsSinceEpoch
                );
            }
        );
    }

private:
    Q_SIGNAL void
    InvalidationSignal(const LibraryInvalidationTargetBitset&);

    // NOLINTBEGIN(cppcoreguidelines-missing-std-forward)
    template <typename ControllerType, typename ControllerRefreshMethodType>
        requires std::is_nothrow_invocable_v<ControllerRefreshMethodType&>
    void
    ConnectInvalidationSignal(
        ControllerType* ControllerPointer,
        const LibraryInvalidationTargetEnum ControllerLibraryInvalidationTarget,
        ControllerRefreshMethodType&& ControllerRefreshMethod
    ) {
        std::invoke(ControllerRefreshMethod);
        QObject::connect(
            this,
            &LibraryInvalidationChannel::InvalidationSignal,
            ControllerPointer,
            [ControllerLibraryInvalidationTarget,
                CapturedControllerRefreshMethod{ std::forward<
                    ControllerRefreshMethodType>(ControllerRefreshMethod) }](
                const LibraryInvalidationTargetBitset&
                    CoordinatorLibraryInvalidationTargetBitset
            ) noexcept -> void {
                if (CoordinatorLibraryInvalidationTargetBitset
                        .Contains(ControllerLibraryInvalidationTarget)) {
                    std::invoke(CapturedControllerRefreshMethod);
                }
            },
            Qt::DirectConnection
        );
    }

    // NOLINTEND(cppcoreguidelines-missing-std-forward)
};

}
#endif

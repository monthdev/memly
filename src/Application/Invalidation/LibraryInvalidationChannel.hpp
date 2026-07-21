#pragma once

#include <qdatetime.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>

#include "Application/Invalidation/LibraryInvalidationTarget.hpp"

namespace Application::Invalidation {

class LibraryInvalidationCoordinator;

class LibraryInvalidationChannel final : public QObject {
    Q_OBJECT
    friend class LibraryInvalidationCoordinator;

private:
    std::int64_t m_CurrentSnapshotAsOfMillisecondsSinceEpoch;

public:
    explicit LibraryInvalidationChannel(QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_CurrentSnapshotAsOfMillisecondsSinceEpoch{ static_cast<std::int64_t>(QDateTime::currentMSecsSinceEpoch()) } {
    }

    explicit LibraryInvalidationChannel(const LibraryInvalidationChannel&) = delete;
    explicit LibraryInvalidationChannel(LibraryInvalidationChannel&&) = delete;
    auto operator=(const LibraryInvalidationChannel&) -> LibraryInvalidationChannel& = delete;
    auto operator=(LibraryInvalidationChannel&&) -> LibraryInvalidationChannel& = delete;

    template <typename ControllerType, typename ControllerRefreshMethodType>
        requires std::is_member_function_pointer_v<ControllerRefreshMethodType> and std::is_nothrow_invocable_v<ControllerRefreshMethodType, ControllerType*>
    void Connect(ControllerType* ControllerPointer,
                 const LibraryInvalidationTargetEnum ControllerLibraryInvalidationTarget,
                 const ControllerRefreshMethodType ControllerRefreshMethod) {
        ConnectInvalidationSignal(ControllerPointer, ControllerLibraryInvalidationTarget, [ControllerPointer, ControllerRefreshMethod]() noexcept -> void {
            std::invoke(ControllerRefreshMethod, ControllerPointer);
        });
    }

    template <typename ControllerType, typename ControllerRefreshMethodType>
        requires std::is_member_function_pointer_v<ControllerRefreshMethodType> and
                 std::is_nothrow_invocable_v<ControllerRefreshMethodType, ControllerType*, std::int64_t>
    void ConnectSnapshot(ControllerType* ControllerPointer,
                         const LibraryInvalidationTargetEnum ControllerLibraryInvalidationTarget,
                         const ControllerRefreshMethodType ControllerRefreshMethod) {
        ConnectInvalidationSignal(
            ControllerPointer, ControllerLibraryInvalidationTarget, [this, ControllerPointer, ControllerRefreshMethod]() noexcept -> void {
                std::invoke(ControllerRefreshMethod, ControllerPointer, m_CurrentSnapshotAsOfMillisecondsSinceEpoch);
            });
    }

private:
    Q_SIGNAL void InvalidationSignal(const LibraryInvalidationTargetBitset&);

    template <typename ControllerType, typename ControllerRefreshMethodType>
        requires std::is_nothrow_invocable_v<ControllerRefreshMethodType&>
    void ConnectInvalidationSignal(ControllerType* ControllerPointer,
                                   const LibraryInvalidationTargetEnum ControllerLibraryInvalidationTarget,
                                   ControllerRefreshMethodType&& ControllerRefreshMethod) {
        std::invoke(ControllerRefreshMethod);
        QObject::connect(
            this,
            &LibraryInvalidationChannel::InvalidationSignal,
            ControllerPointer,
            [ControllerLibraryInvalidationTarget, CapturedControllerRefreshMethod{ std::forward<ControllerRefreshMethodType>(ControllerRefreshMethod) }](
                const LibraryInvalidationTargetBitset& CoordinatorLibraryInvalidationTargetBitset) noexcept -> void {
                if (CoordinatorLibraryInvalidationTargetBitset.Contains(ControllerLibraryInvalidationTarget)) {
                    std::invoke(CapturedControllerRefreshMethod);
                }
            },
            Qt::DirectConnection);
    }
};

}

#pragma once

#include <QDateTime>
#include <QObject>
#include <QtTypes>
#include <functional>
#include <type_traits>
#include <utility>

#include "Application/Invalidation/LibraryInvalidationTarget.hpp"

namespace Application::Invalidation {

class LibraryInvalidationCoordinator;

class LibraryInvalidationChannel final : public QObject {
    Q_OBJECT

public:
    explicit LibraryInvalidationChannel(QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_CurrentSnapshotAsOfMillisecondsSinceEpoch{ QDateTime::currentMSecsSinceEpoch() } {
    }

    ~LibraryInvalidationChannel() noexcept override = default;
    LibraryInvalidationChannel(const LibraryInvalidationChannel&) = delete;
    LibraryInvalidationChannel(LibraryInvalidationChannel&&) = delete;
    LibraryInvalidationChannel& operator=(const LibraryInvalidationChannel&) = delete;
    LibraryInvalidationChannel& operator=(LibraryInvalidationChannel&&) = delete;

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
                 std::is_nothrow_invocable_v<ControllerRefreshMethodType, ControllerType*, qint64>
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

    friend class LibraryInvalidationCoordinator;

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
            [ControllerLibraryInvalidationTarget, ControllerRefreshMethod = std::forward<ControllerRefreshMethodType>(ControllerRefreshMethod)](
                const LibraryInvalidationTargetBitset& CoordinatorLibraryInvalidationTargetBitset) noexcept -> void {
                if (CoordinatorLibraryInvalidationTargetBitset.Contains(ControllerLibraryInvalidationTarget)) {
                    std::invoke(ControllerRefreshMethod);
                }
            },
            Qt::DirectConnection);
    }

    qint64 m_CurrentSnapshotAsOfMillisecondsSinceEpoch;
};

}

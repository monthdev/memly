#pragma once

#include <QDateTime>
#include <QObject>
#include <QtTypes>
#include <concepts>
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

    ~LibraryInvalidationChannel() override = default;
    LibraryInvalidationChannel(const LibraryInvalidationChannel&) = delete;
    LibraryInvalidationChannel(LibraryInvalidationChannel&&) = delete;
    LibraryInvalidationChannel& operator=(const LibraryInvalidationChannel&) = delete;
    LibraryInvalidationChannel& operator=(LibraryInvalidationChannel&&) = delete;

    template <typename ControllerType, typename ControllerRefreshFunctionType>
        requires std::is_member_function_pointer_v<ControllerRefreshFunctionType> and std::invocable<ControllerRefreshFunctionType, ControllerType*>
    void Connect(ControllerType* ControllerPointer,
                 const LibraryInvalidationTargetEnum ControllerLibraryInvalidationTarget,
                 const ControllerRefreshFunctionType ControllerRefreshFunction) {
        ConnectInvalidationSignal(ControllerPointer, ControllerLibraryInvalidationTarget, [ControllerPointer, ControllerRefreshFunction]() -> void {
            std::invoke(ControllerRefreshFunction, ControllerPointer);
        });
    }

    template <typename ControllerType, typename ControllerRefreshFunctionType>
        requires std::is_member_function_pointer_v<ControllerRefreshFunctionType> and std::invocable<ControllerRefreshFunctionType, ControllerType*, qint64>
    void ConnectSnapshot(ControllerType* ControllerPointer,
                         const LibraryInvalidationTargetEnum ControllerLibraryInvalidationTarget,
                         const ControllerRefreshFunctionType ControllerRefreshFunction) {
        ConnectInvalidationSignal(ControllerPointer, ControllerLibraryInvalidationTarget, [this, ControllerPointer, ControllerRefreshFunction]() -> void {
            std::invoke(ControllerRefreshFunction, ControllerPointer, m_CurrentSnapshotAsOfMillisecondsSinceEpoch);
        });
    }

private:
    Q_SIGNAL void InvalidationSignal(const LibraryInvalidationTargetBitset&);

    friend class LibraryInvalidationCoordinator;

    template <typename ControllerType, typename ControllerRefreshFunctionType>
        requires std::invocable<ControllerRefreshFunctionType&>
    void ConnectInvalidationSignal(ControllerType* ControllerPointer,
                                   const LibraryInvalidationTargetEnum ControllerLibraryInvalidationTarget,
                                   ControllerRefreshFunctionType&& ControllerRefreshFunction) {
        std::invoke(ControllerRefreshFunction);
        QObject::connect(
            this,
            &LibraryInvalidationChannel::InvalidationSignal,
            ControllerPointer,
            [ControllerLibraryInvalidationTarget, ControllerRefreshFunction = std::forward<ControllerRefreshFunctionType>(ControllerRefreshFunction)](
                const LibraryInvalidationTargetBitset& CoordinatorLibraryInvalidationTargetBitset) -> void {
                if (CoordinatorLibraryInvalidationTargetBitset.Contains(ControllerLibraryInvalidationTarget)) {
                    std::invoke(ControllerRefreshFunction);
                }
            },
            Qt::DirectConnection);
    }

    qint64 m_CurrentSnapshotAsOfMillisecondsSinceEpoch;
};

}

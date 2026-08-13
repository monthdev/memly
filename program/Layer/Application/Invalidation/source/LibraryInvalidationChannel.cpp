// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Memly/Invalidation/LibraryInvalidationChannel.hpp"

#include <qdatetime.h>
#include <qobject.h>

#include <cstdint>

#include "Memly/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Application::Invalidation {

LibraryInvalidationChannel::LibraryInvalidationChannel(QObject* Parent)
    : QObject{ Parent }
    , Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
    , m_CurrentSnapshotAsOfMillisecondsSinceEpoch{ static_cast<std::int64_t>(QDateTime::currentMSecsSinceEpoch()) } {
}

}
#endif

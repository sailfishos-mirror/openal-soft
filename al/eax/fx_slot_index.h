#ifndef EAX_FX_SLOT_INDEX_INCLUDED
#define EAX_FX_SLOT_INDEX_INCLUDED


#include <cstddef>
#include <optional>
#include <string_view>

#include "api.h"


using EaxFxSlotIndexValue = std::size_t;

class EaxFxSlotIndex : public std::optional<EaxFxSlotIndexValue> {
public:
    using std::optional<EaxFxSlotIndexValue>::optional;

    EaxFxSlotIndex& operator=(const EaxFxSlotIndexValue &value) { set(value); return *this; }
    EaxFxSlotIndex& operator=(const GUID &guid) { set(guid); return *this; }

    void set(EaxFxSlotIndexValue index);
    void set(const GUID& guid);

private:
    [[noreturn]]
    static void fail(const std::string_view message);
}; // EaxFxSlotIndex

inline bool operator==(const EaxFxSlotIndex& lhs, const EaxFxSlotIndex& rhs) noexcept
{
    if(lhs.has_value() != rhs.has_value())
        return false;
    if(lhs.has_value())
        return *lhs == *rhs;
    return true;
}

inline bool operator!=(const EaxFxSlotIndex& lhs, const EaxFxSlotIndex& rhs) noexcept
{ return !(lhs == rhs); }

#endif // !EAX_FX_SLOT_INDEX_INCLUDED

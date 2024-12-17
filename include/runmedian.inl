#include "rqueue.hpp"
#include "runmedian.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <stdbool.h>

namespace common {

#define HANDLE_ERROR(condition, retval)                                                                                \
    do {                                                                                                               \
        if (!(condition) && nullptr != error_cb_) {                                                                    \
            error_cb_();                                                                                               \
            return retval;                                                                                             \
        }                                                                                                              \
    } while (0)

#define HANDLE_ERRORV(condition)                                                                                       \
    do {                                                                                                               \
        if (!(condition) && nullptr != error_cb_) {                                                                    \
            error_cb_();                                                                                               \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0)

template <typename TRMValueType, uint8_t kSize> uint8_t Runmedian<TRMValueType, kSize>::Size() const {
    return values_count;
}

template <typename TRMValueType, uint8_t kSize> bool Runmedian<TRMValueType, kSize>::IsEmpty() const {
    return values_count == 0;
}

template <typename TRMValueType, uint8_t kSize>
void Runmedian<TRMValueType, kSize>::RegisterCallbacks(ErrorCb error_cb, LockCb lock_cb, UnlockCb unlock_cb) {
    error_cb_ = error_cb;
    lock_cb_ = lock_cb;
    unlock_cb_ = unlock_cb;
}

template <typename TRMValueType, uint8_t kSize> TRMValueType Runmedian<TRMValueType, kSize>::Value() const {
    TRMValueType retval{};

    HANDLE_ERROR(values_count <= kSize, retval);

    CONTAINER_LOCK();
    switch (values_count) {
    case 0:
        break;

    case 1:
        retval = values_sorted[0];
        break;

    default:
        const uint8_t even = values_count % 2;
        const uint8_t index = (uint8_t)((values_count >> 1) + even - 1); // half items
        retval = even ? values_sorted[index] : (TRMValueType)((values_sorted[index] + values_sorted[index + 1]) >> 1);
        break;
    }
    CONTAINER_UNLOCK();

    return retval;
}

template <typename TRMValueType, uint8_t kSize> bool Runmedian<TRMValueType, kSize>::_check_integrity() {
    if (values_count < kSize)
        return true;

    TRMValueType qcopy[kSize];
    uint8_t i;
    for (i = 0; i < kSize; i++)
        qcopy[i] = values_queue[i];

    std::sort(qcopy, qcopy + kSize);
    for (i = 0; i < kSize; i++)
        if (qcopy[i] != values_sorted[i])
            return false;

    return true;
}

template <typename TRMValueType, uint8_t kSize> void Runmedian<TRMValueType, kSize>::Add(TRMValueType val) {
    CONTAINER_LOCK();

    HANDLE_ERRORV(values_count <= kSize);
    TRMValueType* ibegin = values_sorted;
    TRMValueType* iend = ibegin + values_count;
    TRMValueType* igreater = std::upper_bound(ibegin, iend, val);
    TRMValueType* ierase = std::find(ibegin, iend, values_queue.Head());

    values_queue.Add(val);

    if (values_count + 1 > kSize) {
        // we should always find the head from queue at array
        HANDLE_ERRORV(ierase < iend && ierase >= ibegin);
        // erase and insert
        if (ierase > igreater) {
            // erase from the right side after igreater
            TRMValueType old = *igreater;
            TRMValueType newold{};
            bool has_newold = false;
            *igreater = val;

            for (TRMValueType* i = igreater + 1; i < iend; i++) {
                if (i == ierase) {
                    *i = old;
                    has_newold = false;
                    break; // do nothing after ierase
                }

                newold = *i;
                has_newold = true;
                *i = old;
                old = newold;
            }
            // check last item
            if (has_newold)
                *(iend - 1) = newold;
        } else {
            if (igreater == ierase) {
                // ierase is always inside array
                HANDLE_ERRORV(ierase < iend);
                *igreater = val;
            } else {
                // erase before insert (ierase < igreater < ibegin + values_count)
                for (TRMValueType* i = ierase; i < igreater - 1; i++) { // do shift before insert
                    HANDLE_ERRORV(i + 1 < iend);
                    *i = *(i + 1);
                }
                // insert new value
                HANDLE_ERRORV(igreater - 1 >= ibegin);
                *(igreater - 1) = val;
            }
        }
    } else {
        // nothing to erase, only insert new value
        if (igreater != iend) {
            TRMValueType old = val;
            // do shift right
            while (igreater < iend + 1) {
                TRMValueType newold = *igreater;
                *igreater = old;
                old = newold;
                igreater++;
            }
        } else { // just add next value
            HANDLE_ERRORV(values_count < kSize);
            ibegin[values_count] = val;
        }

        values_count++;
        HANDLE_ERRORV(values_count <= kSize);
    }
    CONTAINER_UNLOCK();
}

template <typename TRMValueType, uint8_t kSize> void Runmedian<TRMValueType, kSize>::Clear() {
    CONTAINER_LOCK();
    values_queue.DeleteAll();
    memset(values_sorted, 0, sizeof(values_sorted));
    values_count = 0;
    CONTAINER_UNLOCK();
}

} // namespace common

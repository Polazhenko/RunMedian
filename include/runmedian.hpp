// RUNMEDIAN_HPP
#pragma once

#include "rqueue.hpp"
#include <cstdint>
#include <stdbool.h>
#include <type_traits>

namespace common {

typedef void (*ErrorCb)(void);

/// @brief Class for running median for set of values
///
/// It uses sorted set of values and a queue of values in order of appearance
/// we do not sort array, we only insert at right place, but we need to remove
/// the oldest value, which is the head of queue.
/// Array is sorted from less to greater value.
///
template <typename TRMValueType, const uint8_t kSize> class Runmedian {
  public:
    Runmedian() = default;
    static_assert(std::is_arithmetic<TRMValueType>::value, "TRMValueType must be numeric");

    /*
     * @brief if you need to use lock function for multithreading and error handling
     */
    void RegisterCallbacks(ErrorCb error_cb = nullptr, LockCb lock_cb = nullptr, UnlockCb unlock_cb = nullptr);

    /**
     * @brief running median value
     */
    TRMValueType Value() const;

    /**
     * @brief Returns a size of values set, being used for calculating running median.
     */
    uint8_t Size() const;

    /**
     * @brief checks that we have at least one value for calculating median
     */
    bool IsEmpty() const;

    /**
     * @brief Adds an object to set of values.
     *
     * @param container another value for calculating running median.
     */
    void Add(TRMValueType container);

    /**
     * @brief Deletes all objects from the set of values.
     */
    void Clear();

    /**
     * @brief Checks that values at queue are the same as at array
     * @note for unit tests only! CPU bound function
     */
    bool _check_integrity();

  private:
    TRMValueType values_sorted[kSize]{}; // sorted from less to greater value
    uint8_t values_count{};              // used to fill array from 0 to kSize
    Rqueue<TRMValueType, kSize> values_queue{};

    ErrorCb error_cb_{nullptr};
    LockCb lock_cb_{nullptr};
    UnlockCb unlock_cb_{nullptr};
};

} // namespace common

// Here comes the implementation.
#include "runmedian.inl"

// RUNMEDIAN_END

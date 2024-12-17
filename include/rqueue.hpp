// RQUEUE_HPP
#pragma once

#include <stdbool.h>
#include <stdint.h>

namespace common {

typedef void (*LockCb)(void);
typedef void (*UnlockCb)(void);

#define CONTAINER_LOCK()                                                                                               \
    do {                                                                                                               \
        if (nullptr != lock_cb_) {                                                                                     \
            lock_cb_();                                                                                                \
        }                                                                                                              \
    } while (0)

#define CONTAINER_UNLOCK()                                                                                             \
    do {                                                                                                               \
        if (nullptr != unlock_cb_) {                                                                                   \
            unlock_cb_();                                                                                              \
        }                                                                                                              \
    } while (0)

/// @brief Class for circular/round constant queue objects handling.
template <typename T, const uint8_t kSize> class Rqueue {
  public:
    Rqueue() = default;

    void RegisterCallbacks(LockCb lock_cb = nullptr, UnlockCb unlock_cb = nullptr);

    /**
     * @brief Returns a reference to a queue's head object.
     */
    const T& Head();

    /**
     * @brief Returns a reference to a queue's tail object.
     */
    const T& Tail();

    /**
     * @brief Returns a queue's object by index.
     */
    const T& operator[](uint8_t index);

    /**
     * @brief Returns a queue's current size.
     */
    uint8_t Size();

    /**
     * @brief Adds an object to the queue with replacement of the oldest
     * one if the queue is full.
     *
     * @param container an object of queue<T> type.
     */
    void Add(T container);

    /**
     * @brief Adds an object to the queue only if the queue is not full.
     *
     * @param container an object of queue<T> type.
     */
    bool TryAdd(T container);

    /**
     * @brief Deletes an object from head (by moving head).
     */
    void DeleteHead();

    /**
     * @brief Deletes an object from tail (by shrinking size).
     */
    void DeleteTail();

    /**
     * @brief Deletes all objects from the queue.
     */
    void DeleteAll();

  private:
    void AddToTail(T container);
    void IncrementPos();

    T queue_[kSize]{};
    uint8_t queue_pos_{};
    uint8_t queue_size_{};

    LockCb lock_cb_{nullptr};
    UnlockCb unlock_cb_{nullptr};
};

} // namespace common

// Here comes the implementation.
#include "rqueue.inl"

// RQUEUE_END

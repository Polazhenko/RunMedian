// RQUEUE_INL
// As a compromise to have template implementation in separate file but still make it visible to a translation unit.
#pragma once

#include "rqueue.hpp"

namespace common {

template <typename T, uint8_t kSize> void Rqueue<T, kSize>::IncrementPos() {
    queue_pos_ = (uint8_t)((queue_pos_ + 1U) % kSize);
}

template <typename T, uint8_t kSize> void Rqueue<T, kSize>::AddToTail(T container) {
    if (queue_size_ < kSize) {
        queue_size_++;
    } else {
        IncrementPos();
    }

    queue_[(uint8_t)((queue_pos_ + queue_size_ - 1U) % kSize)] = container;
}

template <typename T, uint8_t kSize> void Rqueue<T, kSize>::RegisterCallbacks(LockCb lock_cb, UnlockCb unlock_cb) {
    lock_cb_ = lock_cb;
    unlock_cb_ = unlock_cb;
}

template <typename T, const uint8_t kSize> const T& Rqueue<T, kSize>::Head() {

    CONTAINER_LOCK(); // Critical region: Enter

    uint8_t item_idx = queue_pos_;

    CONTAINER_UNLOCK(); // Critical region: Exit

    return queue_[item_idx];
}

template <typename T, uint8_t kSize> const T& Rqueue<T, kSize>::Tail() {

    CONTAINER_LOCK(); // Critical region: Enter

    uint8_t item_idx = (uint8_t)((queue_pos_ + queue_size_ - 1U) % kSize);

    CONTAINER_UNLOCK(); // Critical region: Exit

    return queue_[item_idx];
}

template <typename T, uint8_t kSize> const T& Rqueue<T, kSize>::operator[](uint8_t index) {

    uint8_t item_idx = 0;

    CONTAINER_LOCK(); // Critical region: Enter

    if (index < queue_size_) {
        item_idx = (uint8_t)((queue_pos_ + index) % kSize);
    } else {
        // Safety fuse.
        item_idx = 0U;
    }

    CONTAINER_UNLOCK(); // Critical region: Exit

    return queue_[item_idx];
}

template <typename T, uint8_t kSize> uint8_t Rqueue<T, kSize>::Size() {

    uint8_t qsize = 0;

    CONTAINER_LOCK(); // Critical region: Enter

    qsize = queue_size_;

    CONTAINER_UNLOCK(); // Critical region: Exit

    return qsize;
}

template <typename T, uint8_t kSize> void Rqueue<T, kSize>::Add(T container) {

    CONTAINER_LOCK(); // Critical region: Enter

    AddToTail(container);

    CONTAINER_UNLOCK(); // Critical region: Exit
}

template <typename T, uint8_t kSize> bool Rqueue<T, kSize>::TryAdd(T container) {

    bool status = false;

    CONTAINER_LOCK(); // Critical region: Enter

    if (queue_size_ < kSize) {
        AddToTail(container);
        status = true;
    } else {
        status = false;
    }

    CONTAINER_UNLOCK(); // Critical region: Exit

    return status;
}

template <typename T, uint8_t kSize> void Rqueue<T, kSize>::DeleteHead() {

    CONTAINER_LOCK(); // Critical region: Enter

    if (queue_size_) {
        queue_size_--;
        IncrementPos();
    }

    CONTAINER_UNLOCK(); // Critical region: Exit
}

template <typename T, uint8_t kSize> void Rqueue<T, kSize>::DeleteTail() {

    CONTAINER_LOCK(); // Critical region: Enter

    if (queue_size_) {
        queue_size_--;
    }

    CONTAINER_UNLOCK(); // Critical region: Exit
}

template <typename T, uint8_t kSize> void Rqueue<T, kSize>::DeleteAll() {

    CONTAINER_LOCK(); // Critical region: Enter

    queue_size_ = 0U;
    queue_pos_ = 0U;

    CONTAINER_UNLOCK(); // Critical region: Exit
}

} // namespace common

// RQUEUE_INL

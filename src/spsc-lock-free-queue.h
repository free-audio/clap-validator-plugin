/*
 * Clap Validator Plugin
 *
 * A expanding set of plugins to test your host
 *
 * Copyright 2024-2025,Various authors, as described in the github
 * transaction log.
 *
 * This source repo is released under the MIT license,
 * The source code and license are at https://github.com/free-audio/clap-validator-plugin
 */

#ifndef FREEAUDIO_CVP_SPSC_LOCK_FREE_QUEUE_H
#define FREEAUDIO_CVP_SPSC_LOCK_FREE_QUEUE_H

#include <atomic>
#include <array>
#include <iostream>
#include <optional>

namespace free_audio::cvp::detail
{
template <typename T, size_t N> class spsc_lockfree_queue
{
  private:
    std::array<T, N> buffer; // Circular buffer storage

    std::atomic<size_t> head{0}; // Read index
    std::atomic<size_t> tail{0}; // Write index

    size_t nextIndex(size_t index) const
    {
        return (index + 1) % N; // Circular buffer index wrapping
    }

  public:
    bool enqueue(const T &item)
    {
        size_t currentTail = tail.load(std::memory_order_relaxed);
        size_t nextTail = nextIndex(currentTail);

        if (nextTail == head.load(std::memory_order_acquire))
        {
            return false; // Queue is full
        }

        buffer[currentTail] = item; // Copy new item into the buffer

        tail.store(nextTail, std::memory_order_release);
        return true;
    }

    // Dequeue an item (returns std::optional or std::nullopt if queue is empty)
    std::optional<T> dequeue()
    {
        size_t currentHead = head.load(std::memory_order_relaxed);

        // Check if queue is empty
        if (currentHead == tail.load(std::memory_order_acquire))
        {
            return std::nullopt; // Queue is empty
        }

        T item = buffer[currentHead]; // Retrieve item from buffer

        // Update head pointer
        head.store(nextIndex(currentHead), std::memory_order_release);
        return item;
    }

    std::optional<T> dequeueMostRecent()
    {
        size_t currentHead = head.load(std::memory_order_relaxed);
        size_t currentTail = tail.load(std::memory_order_acquire);
        // Check if queue is empty
        if (currentHead == currentTail)
        {
            return std::nullopt; // Queue is empty
        }

        if (currentTail == 0)
            currentHead = N - 1;
        else
            currentHead = currentTail - 1;

        T item = buffer[currentHead]; // Retrieve item from buffer

        // Update head pointer
        head.store(nextIndex(currentHead), std::memory_order_release);
        return item;
    }
};
} // namespace free_audio::cvp::detail
#endif // SPSC_LOCK_FREE_QUEUE_H

/**
 * @file blocking_queue.hpp
 * @author Fansure Grin
 * @date 2024-08-31
 * @brief blocking queue
*/
#ifndef BLOCKING_QUEUE_HPP
#define BLOCKING_QUEUE_HPP

#include <deque>
#include <condition_variable>
#include <mutex>

namespace racoon {

template <typename T>
class BlockingQueue {
private:
    using lock_guard = std::lock_guard<std::mutex>;
    using unique_lock = std::unique_lock<std::mutex>;
public:
    using size_type = typename std::deque<T>::size_type;
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;

    BlockingQueue(size_type max_cap = 256);
    ~BlockingQueue();

    value_type front();
    value_type back();

    bool empty() noexcept;
    bool full() noexcept;
    size_type size() noexcept;
    size_type capacity() noexcept;

    void clear() noexcept;
    void push(const T &ele);
    bool pop(T &ele);

    void flush();
    void close();
private:
    std::deque<T> deq;
    std::mutex mtx;
    size_type cap;
    std::condition_variable producer;
    std::condition_variable consumer;
    bool is_close;
};

template <typename T>
inline BlockingQueue<T>::BlockingQueue(size_type max_cap)
: cap(max_cap) {
    is_close = false;
}

template <typename T>
BlockingQueue<T>::~BlockingQueue() {
    close();
}

template <typename T>
typename BlockingQueue<T>::value_type BlockingQueue<T>::front() {
    lock_guard lck(mtx);
    return deq.front();
}

template <typename T>
typename BlockingQueue<T>::value_type BlockingQueue<T>::back() {
    lock_guard lck(mtx);
    return deq.front();
}

template <typename T>
bool BlockingQueue<T>::empty() noexcept {
    lock_guard lck(mtx);
    return deq.empty();
}

template <typename T>
bool BlockingQueue<T>::full() noexcept {
    lock_guard lck(mtx);
    return deq.size() >= cap;
}

template <typename T>
typename BlockingQueue<T>::size_type
BlockingQueue<T>::size() noexcept {
    lock_guard lck(mtx);
    return deq.size();
}

template <typename T>
typename BlockingQueue<T>::size_type
BlockingQueue<T>::capacity() noexcept {
    lock_guard lck(mtx);
    return cap;
}

template <typename T>
void BlockingQueue<T>::clear() noexcept {
    lock_guard lck(mtx);
    deq.clear();
}

template <typename T>
void BlockingQueue<T>::push(const T &ele) {
    unique_lock lck(mtx);
    producer.wait(lck, [this]{ return is_close || deq.size() < cap; });
    if (is_close) return;
    deq.push_back(ele);
    consumer.notify_one();
}

template <typename T>
bool BlockingQueue<T>::pop(T &ele) {
    unique_lock lck(mtx);
    consumer.wait(lck, [this]{ return is_close || !deq.empty(); });
    if (is_close) return false;
    ele = deq.front();
    deq.pop_front();
    producer.notify_one();
    return true;
}

template <typename T>
void BlockingQueue<T>::flush() {
    consumer.notify_one();
}

template <typename T>
void BlockingQueue<T>::close() {
    {
        lock_guard lck(mtx);
        deq.clear();
        is_close = true;
    }
    producer.notify_all();
    consumer.notify_all();
}

}

#endif // end of BLOCKING_QUEUE_HPP
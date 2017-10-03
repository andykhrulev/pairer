//
// Created by Andrey Khrulev on 24.09.17.
//

#ifndef SAMPLEPROJECT_BLOCKINGDEQUEUE_H
#define SAMPLEPROJECT_BLOCKINGDEQUEUE_H

#include <mutex>
#include <condition_variable>
#include <deque>

namespace pairer {
    namespace utils {

        /** Blocking queue is thread safe implementation of dequeue.
         * The class allow reader to perform timed wait till new element in queue.
         *
         * @tparam T Type that will be queued
         */
        template<typename T>
        class BlockingDequeue {
        public:
            /** Adds element to end of queue.
             *
             * @param value object to be added
             */
            void push_back(T const &value) {
                {
                    std::unique_lock<std::mutex> lock(this->mutex_);
                    queue_.push_back(value);
                }
                this->condition_.notify_one();
            }

            /** Removes first element of the queue.
             *
             */
            void pop_front() {
                std::unique_lock<std::mutex> lock(this->mutex_);
                this->condition_.wait(lock, [this] { return !this->queue_.empty(); });
                this->queue_.pop_front();
            }

            /** Returns copy of first object in queue without deleting it.
             *
             * @return reference to object
             */
            T &front() {
                std::unique_lock<std::mutex> lock(this->mutex_);
                this->condition_.wait(lock, [this] { return !this->queue_.empty(); });
                return this->queue_.front();
            }

            /** Removes object from the queue and return it for first object where predicate returns true
             *
             * @param pred predicate function that returns true if object needs to be deleted from queue
             * @param value value is filled with object that predicate returns true. object is removed from queue
             * @return true, if element is retrieved, false - otherwise
             */
            bool retrieve_one_if(T &value, std::function<bool(T &)> const &pred) {
                for (auto it = this->queue_.begin(); it != this->queue_.end(); it++) {
                    if (pred(*it)) {
                        value = *it;
                        this->queue_.erase(it);
                        return true;
                    }
                }
                return false;
            }

        private:
            std::mutex mutex_;
            std::condition_variable condition_;
            std::deque<T> queue_;
        };
    }
}


#endif //SAMPLEPROJECT_BLOCKINGDEQUEUE_H

//
// Created by Andrey Khrulev on 25.09.17.
//

#ifndef SAMPLEPROJECT_TIMERPOOL_H
#define SAMPLEPROJECT_TIMERPOOL_H

#include <string>
#include <thread>
#include "BlockingDequeue.h"
#include "log.h"

namespace pairer {
    namespace utils {

        /** Interface/Abstract class for timeout handling
         *
         * @tparam T data type of timer identifier
         */
        template<typename T>
        class TimeoutHandler {
        public:
            virtual ~TimeoutHandler() {}

            /** Processes timeout for stored timer
             *
             * @param id data that identifies timer for client
             */
            virtual void onTimerExpiry(T id) = 0;
        };

        template<typename T>
        /** Allows user to raise timers with particular delay
         *
         * @tparam T data type of timer identifier
         *
         */
        class TimerPool {
        // TODO: rework pool, so handler and duration can be provided for each item so pool can be used by several client objects
        public:
            /** constructs TimerPool
             *
             * @param handler  object that will be called in case timer expiration
             * @param duration duration for timer expiration (one for all timers for simplicity)
             */
            TimerPool(TimeoutHandler<T> *handler, const std::chrono::milliseconds &duration) : handler_(handler),
                                                                                         duration_(duration) {
                active_ = false;
            };

            /** Reset timeout handler to another handler
             *
             * @param handler object that will be triggered in case timer expiration
             */
            void setTimeoutHandler(TimeoutHandler<T> *handler) {
                handler_ = handler;
            }

            virtual ~TimerPool() { stop(); }

            /** adds new timer
             *
             * @param id identifies timer
             */
            void addNewTimer(T id) {
                queue_.push_back(t_pair(std::chrono::steady_clock::now()+duration_, id));
            };

            /** deletes timer from pool
             *
             * @param id identifies timer
             * @return true, if timer was deleted, false - if no such timer found in pool
             */
            bool deleteTimer(T id) {
                t_pair pair;
                return queue_.retrieve_one_if(pair, [&id](t_pair &pair) { return 0 == pair.second.compare(id);});
            }

            /** Starts main thread
             *
             */
            void start() {
                LOGGER_IN();
                if (active_) {
                    LOGGER_STR("ERROR: Pool is already started")
                    return;
                }
                active_ = true;
                timerthread_ = std::thread(&TimerPool::timerPoolProcessor, this);
            }

            /** Stops main thread
             *
             */
            void stop() {
                LOGGER_IN()
                if (!active_) {
                    LOGGER_STR("ERROR: Pool is not started");
                    return;
                }

                active_ = false;

                // in case queue is empty then pool is sleeping on a queue - it needs to be awaken
                // below is hack to unblock pool on queue
                // TODO: update queue to do add timed wait abilities so thread is awaken periodically
                T id;
                addNewTimer(id);
                timerthread_.join();
            }

        protected:
            /** main process for timer pool
             *
             */
            void timerPoolProcessor() {
                LOGGER_IN()
                while (active_) {
                    //
                    //1. Timeout - raise event for expired timers
                    //
                    // there is small chance that no timer will be expired - in case timers to be expired
                    // are removed from pool before expiration
                    //
                    t_timepoint current_time = std::chrono::steady_clock::now(); // get current time
                    t_pair expiredPair;

                    // retrieve expired pairs (where timeout time is less then current) one by one
                    // and trigger timeout handler with expired event
                    //
                    while (queue_.retrieve_one_if(expiredPair, [&current_time, &log](t_pair &pair) {
                            return (current_time > pair.first);
                        })) {
                        handler_->onTimerExpiry(expiredPair.second);
                    }


                    //
                    // 2. calculate time for new timeout
                    //
                    // Assuming that timers have equal duration, so the first in the queue is nearest for expiration
                    auto expiration_time = queue_.front().first;
                    current_time = std::chrono::steady_clock::now(); // get current time
                    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(expiration_time-current_time);

                    std::this_thread::sleep_for(duration); //it is might be a small
                }
            }

        protected:
            using t_timepoint = std::chrono::time_point<std::chrono::steady_clock>;
            using t_pair = std::pair<t_timepoint, T>;

            BlockingDequeue<t_pair> queue_;  // queue of pairs: time_to_expiry and id, that identifies timer
            TimeoutHandler<T> *handler_;    // handler for timers that to be timed out
            std::thread timerthread_;       // internal thread
            std::chrono::milliseconds duration_;  // duration for timeout
            bool active_; // indicator of activity for the thread process
        };
    }
}

#endif //SAMPLEPROJECT_TIMERPOOL_H

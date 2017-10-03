//
// Created by Andrey Khrulev on 01.10.17.
//

#ifndef SAMPLEPROJECT_APPSTATS_H
#define SAMPLEPROJECT_APPSTATS_H


#include <string>
#include <vector>
#include "TimerPool.h"

namespace pairer {
    namespace utils {

/** Manages statistic counters
 *
 */
        class AppStats : public pairer::utils::TimeoutHandler<std::string> {
        public:
            /** constructs object
             *
             * @param number_stats number of metric counters to be supported by object
             * @param active_counter_duration time period for counting metric in active register, after that period counted value is moved to hold register, active register is zeroed
             */
            AppStats(int number_stats, const std::chrono::milliseconds &active_counter_duration);

            /** destructor
             *
             */
            virtual ~AppStats() {};

            /** increments selected counter (in active register)
             *
             * @param counter_id identifies metric to be incremented
             */
            virtual void incrementCounter(int counter_id);

            /** returns value counted in active register since last data movement (to hold)
             *
             * @param counter_id identifies metric
             * @return metric counter from active register
             */
            virtual uint32_t getActiveCounter(int counter_id);

            /** returns value counted for metric during last active_counter_duration
             * this value is updated from active register each active_counter_duration msecs.
             * @param counter_id identifies metric
             * @return metric counter from hold register
             */
            virtual uint32_t getHoldingCounter(int counter_id);

            /** active_counter_duration timeout callback
             *
             * @param id unused
             */
            void onTimerExpiry(std::string id) override;

        protected:
            /** Moves counter values from active to holding register.
             * Can be used to report calculated values during last period
             */
            virtual void moveActiveToHold();

        protected:
            std::vector<std::atomic<uint32_t>> active_counters_; // active register
            std::vector<uint32_t> holding_counters_; // holding register
            int number_stats_;                                   // number of supported metric counters
            pairer::utils::TimerPool<std::string> pool_;           // timer pool
        };
    }
}

#endif //SAMPLEPROJECT_APPSTATS_H

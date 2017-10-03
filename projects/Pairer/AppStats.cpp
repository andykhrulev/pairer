//
// Created by Andrey Khrulev on 01.10.17.
//

#include "AppStats.h"
#include "catch.hpp"

using namespace pairer::utils;

AppStats::AppStats(int number_stats, const std::chrono::milliseconds &active_counter_duration):
        number_stats_(number_stats), active_counters_(number_stats), holding_counters_(number_stats),
        pool_(this, active_counter_duration) {
    // start timer pool
    pool_.start();
    // set timer
    pool_.addNewTimer(std::string());
}

void AppStats::onTimerExpiry(std::string id) {
    //re-add timer
    pool_.addNewTimer(std::string());

    // move active counters to hold ones
    moveActiveToHold();
}

void AppStats::incrementCounter(int counter_id) {
    if (counter_id >= number_stats_) {
        return; // just ignore - stats will not be collected for the counter. shall exception to be raised?
    }
    active_counters_[counter_id]++;
}

uint32_t AppStats::getActiveCounter(int counter_id) {
    if (counter_id >= number_stats_) {
        return 0; // just ignore - stats were not collected for the items. shall exception to be raised?
    }
    return active_counters_[counter_id];
}

uint32_t AppStats::getHoldingCounter(int counter_id) {
    if (counter_id >= number_stats_) {
        return 0; // just ignore - stats were not collected for the items. shall exception to be raised?
    }
    return holding_counters_[counter_id];
}

void AppStats::moveActiveToHold() {
    LOGGER_IN()
    LOGGER_STR("=======STATS=======");

    for(int i=0; i<number_stats_;i++) {
        holding_counters_[i] = active_counters_[i].fetch_and(0);
        LOGGER_STR_ARG2("    stat [%d] = %d", i, holding_counters_[i]);
    }
}

#ifdef CATCH_UNITTEST_ENABLED

TEST_CASE("AppStats", "[utils]") {
    LOGGER_IN()

    pairer::utils::AppStats* stats;

    auto active_to_holding_duration = std::chrono::milliseconds(300);

    // 1. object creation
    REQUIRE_NOTHROW(stats = new AppStats(5, active_to_holding_duration));

    // 2. get counter
    REQUIRE(stats->getActiveCounter(0) == 0);
    REQUIRE(stats->getHoldingCounter(0) == 0);

    // 3. increment counter
    REQUIRE_NOTHROW(stats->incrementCounter(0));
    REQUIRE_NOTHROW(stats->incrementCounter(4));

    REQUIRE(stats->getActiveCounter(0) == 1);
    REQUIRE(stats->getActiveCounter(4) == 1);

    // 4. moving from active to holding
    std::this_thread::sleep_for(active_to_holding_duration*4/3); // wait for copying counters
    REQUIRE(stats->getActiveCounter(0) == 0);
    REQUIRE(stats->getHoldingCounter(0) == 1);

    // 5. moving from active to holding 2
    REQUIRE_NOTHROW(stats->incrementCounter(1));
    REQUIRE_NOTHROW(stats->incrementCounter(1));
    std::this_thread::sleep_for(active_to_holding_duration*4/3); // wait for copying counters
    REQUIRE(stats->getActiveCounter(1) == 0);
    REQUIRE(stats->getHoldingCounter(1) == 2);

    // 4. increment invalid counter
    REQUIRE_NOTHROW(stats->incrementCounter(5));
    REQUIRE(stats->getActiveCounter(5) == 0);

    REQUIRE_NOTHROW(delete stats);
}
#endif

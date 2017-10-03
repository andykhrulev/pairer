//
// Created by Andrey Khrulev on 25.09.17.
//

#include <zconf.h>
#include "TimerPool.h"

#include "log.h"

#include "catch.hpp"

using namespace pairer::utils;

#ifdef CATCH_UNITTEST_ENABLED

class TestTimeoutHandler: public TimeoutHandler<std::string> {
public:
    void onTimerExpiry(std::string id) override {
        LOGGER_IN()
        timers.push_back(id);
    }

public:
    std::vector<std::string> timers;
};

TEST_CASE("TimerPool", "[utils]") {

    //TimeoutHandler<std::string>* handler = new TestTimeoutHandler();
    TestTimeoutHandler* handler = new TestTimeoutHandler();
    TimerPool<std::string>* pool;
    auto test_duration = std::chrono::milliseconds (300);

    // 1. object creation
    REQUIRE_NOTHROW(pool = new TimerPool<std::string>(handler, test_duration));
    REQUIRE_NOTHROW(pool->start());

    std::this_thread::sleep_for(test_duration); // wait for duration before test start

    // 2. adding timer
    REQUIRE_NOTHROW(pool->addNewTimer("12"));
    // 3. deletion timer before timeout
    REQUIRE(true == pool->deleteTimer("12"));

    std::this_thread::sleep_for(test_duration*2); // wait for double duration
    REQUIRE(0 == handler->timers.size()); // nothing should expire - timer pool is empty

    // 4. timer timeout
    REQUIRE_NOTHROW(pool->addNewTimer("12"));
    std::this_thread::sleep_for(test_duration*2); // wait for double duration
    REQUIRE(handler->timers[0] == "12");
    // clear timers array after test
    handler->timers.clear();

    // 5. delete after timer timeout
    REQUIRE(false == pool->deleteTimer("12"));

    // 6. delete from the middle
    REQUIRE_NOTHROW(pool->addNewTimer("12"));
    REQUIRE_NOTHROW(pool->addNewTimer("34"));
    REQUIRE_NOTHROW(pool->addNewTimer("56"));
    REQUIRE_NOTHROW(pool->addNewTimer("Test Name For Timer"));
    REQUIRE(true == pool->deleteTimer("Test Name For Timer"));

    // 7. mass time out
    std::this_thread::sleep_for(test_duration*2); // wait for double duration
    REQUIRE(handler->timers.size() == 3);
    REQUIRE(handler->timers[0] == "12");
    REQUIRE(handler->timers[1] == "34");
    REQUIRE(handler->timers[2] == "56");

    REQUIRE_NOTHROW(delete pool);
}
#endif

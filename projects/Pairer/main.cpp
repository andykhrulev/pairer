#include <iostream>
#include "SimpleSyncReceivedMessageHandler.h"
#include "RadiusMessageReceiver.h"

#include "log.h"
#include "RadiusProxyMsgReceiver.h"
#include "SimpleMapCache.h"
#include "ThriftResultLogger.h"
#include "Config.h"

ENABLE_DEBUG()

using namespace std;

int main() {
#ifdef DEBUG_ENABLED
    ofstream of("pairer.log");
    INIT_DEBUG(&of);
#endif

    using namespace pairer;

    radius::RadiusMessageReceiver* receiver;
    Config config;

    LOGGER_STR_ARG2("RADIUS Receiver: %s:%d",config.receiverIp().c_str(),config.receiverPort());
    LOGGER_STR_ARG2("RADIUS server  : %s:%d",config.radiusServerIp().c_str(),config.radiusServerPort());
    LOGGER_STR_ARG2("Thift server: %s:%d",config.thriftServerIp().c_str(),config.thriftServerPort());

    // use simple cache for storing received requests
    auto cache = std::shared_ptr<cache::DataCache>(new cache::SimpleMapCache());

    // use timer pool to track responses
    auto timer_pool = std::shared_ptr<utils::TimerPool<std::string>>(
            new utils::TimerPool<std::string>(nullptr, config.responseTimeout()));

    // use thrift result logger for reporting
    auto reporter = std::shared_ptr<reporting::ResultReporter>(
            new reporting::ThriftResultLogger(config.appId(),config.thriftServerIp(), config.thriftServerPort()));

    //statistic to be stored each 2 mins (=2 * 60 * 1000 msec)
    auto stats = std::shared_ptr<utils::AppStats>(new utils::AppStats(
            SimpleSyncReceivedMessageHandler::NUM_COUNTERS_USED,config.statisticPeriod()));

    timer_pool->start();

    if (config.proxyMode()) {
        receiver = new radius::RadiusProxyMsgReceiver(
                new SimpleSyncReceivedMessageHandler(cache, timer_pool, reporter, stats),
                config.radiusServerIp(), config.radiusServerPort());
    } else {
        receiver = new radius::RadiusMessageReceiver(
                new SimpleSyncReceivedMessageHandler(cache, timer_pool, reporter, stats));
    }

    receiver->start(config.receiverIp(), config.receiverPort());

    return 0;


}

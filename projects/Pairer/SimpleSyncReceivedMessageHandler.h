//
// Created by Andrey Khrulev on 18.09.17.
//

#ifndef SAMPLEPROJECT_SIMPLESYNCRECEIVEDMESSAGEHANDLER_H
#define SAMPLEPROJECT_SIMPLESYNCRECEIVEDMESSAGEHANDLER_H

#include "MessageHandler.h"
#include "DataCache.h"
#include "TimerPool.h"
#include "ResultReporter.h"
#include "AppStats.h"

namespace pairer {

    /** Simple synchronious message handler for messages received by RadiusMessageReceiver
     * No async mechanisms and loadsharing are used for simplicity
     */
    class SimpleSyncReceivedMessageHandler
            : public pairer::MessageHandler, public pairer::utils::TimeoutHandler<std::string> {

    public:
        /** constructor
         *
         * @param cache cache that will be used for storing received messages
         * @param pool timer pool to track received messages timeouts
         * @param reporter used for reporting about found pairs of messages
         * @param stats statistic storage
         */
        SimpleSyncReceivedMessageHandler(std::shared_ptr<pairer::cache::DataCache> &cache,
                                         std::shared_ptr<pairer::utils::TimerPool<std::string>> &pool,
                                         std::shared_ptr<pairer::reporting::ResultReporter> &reporter,
                                         std::shared_ptr<pairer::utils::AppStats> &stats) :
                msg_cache_(cache), pool_(pool), reporter_(reporter), stats_(stats) {
            // subscribes to timer expiration events
            pool->setTimeoutHandler(this);
        }

        virtual ~SimpleSyncReceivedMessageHandler() {}

        /** handles message received from the Receiver
         *
         * @param msg received message
         */
        void handleMessage(pairer::Message *msg) override;

    private:
        /** handles radius response timeout events
         *
         * @param id identifies the radius request which response is not received for
         */
        void onTimerExpiry(std::string id) override;

    public:
        /** number counters that is used by the class
         *
         */
        const static int NUM_COUNTERS_USED = 3;

    protected:
        // statistic counter ids
        const static int NUM_RECEIVED_MSGS = 0; //number of received messages
        const static int NUM_PAIRED_REQUESTS = 1;      // number of paired messages
        const static int NUM_PAIRING_ATTEMPTS_EXPIRED = 2; //no paired response received

        std::shared_ptr<pairer::cache::DataCache> msg_cache_;
        std::shared_ptr<pairer::utils::TimerPool<std::string>> pool_;
        std::shared_ptr<pairer::reporting::ResultReporter> reporter_;
        std::shared_ptr<pairer::utils::AppStats> stats_;
    };
}

#endif //SAMPLEPROJECT_SIMPLESYNCRECEIVEDMESSAGEHANDLER_H

//
// Created by Andrey Khrulev on 18.09.17.
//

#include "SimpleSyncReceivedMessageHandler.h"
#include "log.h"

using namespace pairer;

void SimpleSyncReceivedMessageHandler::handleMessage(pairer::Message *msg) {
    LOGGER_IN();

    LOGGER_STR_ARG2("Received message (id=%s) is: %s", msg->getSessionID().c_str(), msg->getSerializedValue().c_str());

    stats_->incrementCounter(NUM_RECEIVED_MSGS);

    //Let's look whether request is already in cache
    if (msg_cache_->lookup(msg->getSessionID())) {
        LOGGER_STR("Pair found");
        //get request from cache
        pairer::Message *pair = dynamic_cast<pairer::Message*>(msg_cache_->pop_object(msg->getSessionID()));

        stats_->incrementCounter(NUM_PAIRED_REQUESTS);

        reporter_->reportPair(pair, msg);

        pool_->deleteTimer(pair->getSessionID());
    } else {
        LOGGER_STR("no pair in cache, adding to cache and waiting for pair");
        // No pair in cache yet
        // since there is a small chance that response comes first then let's put any unpaired message to cache
        msg_cache_->add(msg->getKey(), msg);

        pool_->addNewTimer(msg->getSessionID());
    }
}

void SimpleSyncReceivedMessageHandler::onTimerExpiry(std::string id) {
    LOGGER_IN()

    msg_cache_->pop_object(id);
    LOGGER_STR_ARG1("Request with id=%s, timeout expired - no response", id.c_str());

    stats_->incrementCounter(NUM_PAIRING_ATTEMPTS_EXPIRED);
}


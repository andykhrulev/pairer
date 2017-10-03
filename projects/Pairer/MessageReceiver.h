//
// Created by Andrey Khrulev on 14.09.17.
//

#ifndef SAMPLEPROJECT_MESSAGERECEIVER_H
#define SAMPLEPROJECT_MESSAGERECEIVER_H


#include "Message.h"
#include "MessageHandler.h"

namespace pairer {
    class MessageReceiver {
    private:
        MessageReceiver()  = delete;

    public:
        MessageReceiver(MessageReceiver& receiver) {};
        virtual void start() = 0;

        class MessageReceiverException: public std::exception {
        public:
            const char* what () const throw() {
                return "Message Receiver default exception\n";
            }
        };

    protected:
        virtual void getMessageFromStream(Message &receivedMsg) = 0;

    };

};
#endif //SAMPLEPROJECT_MESSAGERECEIVER_H

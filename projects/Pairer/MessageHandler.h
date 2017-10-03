//
// Created by Andrey Khrulev on 14.09.17.
//

#ifndef SAMPLEPROJECT_MESSAGEHANDLER_H
#define SAMPLEPROJECT_MESSAGEHANDLER_H


#include <string>
#include "Message.h"

namespace pairer {

    //
    class MessageHandler {
    protected:
        MessageHandler() {}

    public:
        virtual ~MessageHandler() {}
        virtual void handleMessage(Message *msg) = 0;

    };

};

#endif //SAMPLEPROJECT_MESSAGEHANDLER_H

//
// Created by Andrey Khrulev on 14.09.17.
//

#ifndef SAMPLEPROJECT_MESSAGE_H
#define SAMPLEPROJECT_MESSAGE_H

#include <string>
#include <map>
#include "DataCache.h"

namespace pairer {

    class Message: public pairer::cache::Cachable {
    private:

        //restore Message from serialized value
        Message(std::string &serializedData) = delete;

    protected:
        Message() {};
        virtual ~Message() {};

    public:
        //return id that is equal for request/response messages
        virtual std::string getSessionID() = 0;

        //return attributes
        virtual std::map<short, std::string> getAttributes() = 0;

        //return serialized value
        virtual std::string getSerializedValue() = 0;
    };


};
#endif //SAMPLEPROJECT_MESSAGE_H

//
// Created by Andrey Khrulev on 14.09.17.
//

#ifndef SAMPLEPROJECT_RADIUSMESSAGE_H
#define SAMPLEPROJECT_RADIUSMESSAGE_H


#include "Message.h"
#include "radius.h"

namespace pairer {
namespace radius {
    //shall be singletone but anyway do not expected to be multiple created
    /** Converts freeradius avp values to their string representations
     * 
     */
    class RadiusAttributeBuilder {
    public:
        RadiusAttributeBuilder();

        /** Converts attributes to string reprsentation
         *
         * @param attr_val pointer to attribute
         * @param attr_len attribute len in octets (bytes)
         * @param attr_type type of attribute (string, int, ipv4, etc)
         * @return string representation of avp
         */
        virtual std::string attrToString(const void *attr_val, int attr_len, PW_TYPE attr_type);

    protected:
        static std::string stringToString(const void *attr_val, int attr_len);

        static std::string uint32ToString(const void *attr_val, int attr_len);

        static std::string int32ToString(const void *attr_val, int attr_len);

        static std::string ipv4ToString(const void *attr_val, int attr_len);

        static std::string dateToString(const void *attr_val, int attr_len);

        static std::string octetsToString(const void *attr_val, int attr_len);

        static std::string byteToString(const void *attr_val, int attr_len);

        static std::string ushortToString(const void *attr_val, int attr_len);

        static std::string uint64ToString(const void *attr_val, int attr_len);

        static std::string boolToString(const void *attr_val, int attr_len);

    protected:
        typedef std::string (*AttrToStringFunc)(const void *attr_val, int attr_len);

        std::map<PW_TYPE, AttrToStringFunc> converter;
    };

    /** this is Radius specific message implementation - keeps Radius request/response
     *
     */
    class RadiusMessage : public pairer::Message {

    public:
        RadiusMessage() = delete;

        /** initialized from incoming message
         *
         * @param packet freeradius specific packet received from the network
         * @param attrConverter converter for freeradius attributes
         */
        RadiusMessage(const RADIUS_PACKET &packet, RadiusAttributeBuilder &attrConverter);

        std::string getContent() override;

        std::string getKey() override;

        /** intialises radius message from serialized value
         * 
         * @param serializedData  serialized representation
         */
        RadiusMessage(std::string &serializedData);

        /** copy constructor
         * 
         * @param msg source for copying 
         */
        RadiusMessage(RadiusMessage &msg);

        /** return id that is equal for request/response messages
         * 
         * @return identifier that uniquelly identifies request/response pair
         */
        std::string getSessionID() override;

        /** return attributes
         * 
         * @return map of code to avp values represented by strings 
         */
        std::map<short, std::string> getAttributes() override;

        /** serializes message data
         * 
         * @return serialized representation of packet - may be usefull for storing in blob (database)
         */
        std::string getSerializedValue() override;

    protected:
        /** converts freeradius specific packet data to internal representation (map of code to avp)
         * 
         * @param packet packet data in freeradius format
         * @param attrConverter handler to converter object
         */
        void buildAttrsFromRadiusPacket(const RADIUS_PACKET &packet, RadiusAttributeBuilder &attrConverter);

        /** restores message from serialized value
         * 
         * @param serializedData string representation of data
         */
        void restoreMessage(std::string serializedData);

    protected:
        std::map<short, std::string> attrs_;
        std::string identifier_;
    };
};
};

#endif //SAMPLEPROJECT_RADIUSMESSAGE_H

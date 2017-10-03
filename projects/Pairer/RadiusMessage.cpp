//
// Created by Andrey Khrulev on 14.09.17.
//

#include <sstream>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "RadiusMessage.h"

#include "log.h"

#include "catch.hpp"

using namespace pairer::radius;


std::string RadiusMessage::getSessionID() {
    return identifier_;
}


RadiusMessage::RadiusMessage(std::string &serializedData):Message()  {
    //attrConverter = new RadiusAttributeBuilder();

    std::stringstream ss(serializedData);
    boost::archive::text_iarchive iarch(ss);
    iarch >> attrs_;

}

RadiusMessage::RadiusMessage(const RADIUS_PACKET &packet, RadiusAttributeBuilder& attrConverter) {

    buildAttrsFromRadiusPacket(packet, attrConverter);
}

std::string RadiusMessage::getSerializedValue() {
    LOGGER_IN();

    std::stringstream ss;
    boost::archive::text_oarchive oarch(ss);
    oarch << attrs_;

    LOGGER_STR_ARG1("serialized value: %s", ss.str().c_str());

    return ss.str();
}

void RadiusMessage::buildAttrsFromRadiusPacket(const RADIUS_PACKET& packet, RadiusAttributeBuilder& attrConverter){
    LOGGER_IN();
    identifier_ = std::to_string(packet.id);
    LOGGER_STR_ARG2("packet (id=%s, attrs_=%p)", identifier_.c_str(), packet.vps);

    for (VALUE_PAIR* vps = packet.vps; vps; vps = vps->next) {
        LOGGER_STR_ARG2("avp (%d) = type %d", vps->da->attr, vps->da->type);
        LOGGER_STR_ARG2("  value[0]=%p, len=%ld", vps->data.ptr, vps->length);
        LOGGER_STR_ARG2("  attr type=%d, encoding=%d", vps->type,vps->da->flags.encrypt);
        PW_TYPE type = vps->da->type;
        if (vps->da->flags.encrypt && type == PW_TYPE_STRING) {
            type = PW_TYPE_OCTETS;
        }
        switch(type){
            case PW_TYPE_STRING:             //!< String of printable characters.
            case PW_TYPE_OCTETS:             //!< Binary octets
                attrs_[vps->da->attr] = attrConverter.attrToString(vps->data.ptr, vps->length, type);
                break;
            default:
                attrs_[vps->da->attr] = attrConverter.attrToString(&(vps->data), vps->length, type);
        }
        LOGGER_STR_ARG2("avp [%d]=%s",vps->da->attr,attrs_[vps->da->attr].c_str());
    }
}

std::map<short, std::string> RadiusMessage::getAttributes() {
    return attrs_;
}

std::string RadiusMessage::getKey() {
    return getSessionID();
}

std::string RadiusMessage::getContent() {
    return getSerializedValue();
}

std::string RadiusAttributeBuilder::attrToString(const void *attr_val, int attr_len, PW_TYPE attr_type) {
    LOGGER_IN();
    if (converter.find(attr_type) == converter.end()) {
        // attributes with unknown types will be converted as octet strings
        return converter[PW_TYPE_OCTETS](attr_val, attr_len);
    }
    return converter[attr_type](attr_val, attr_len);
}

RadiusAttributeBuilder::RadiusAttributeBuilder() {
    converter[PW_TYPE_OCTETS] =  &RadiusAttributeBuilder::octetsToString;
    converter[PW_TYPE_STRING] =  &RadiusAttributeBuilder::stringToString;
    converter[PW_TYPE_INTEGER] = &RadiusAttributeBuilder::uint32ToString;
    converter[PW_TYPE_DATE] =    &RadiusAttributeBuilder::dateToString;
    converter[PW_TYPE_IPV4_ADDR]=&RadiusAttributeBuilder::ipv4ToString;
    converter[PW_TYPE_BYTE] =    &RadiusAttributeBuilder::byteToString;
    converter[PW_TYPE_SHORT] =   &RadiusAttributeBuilder::ushortToString;
    converter[PW_TYPE_INTEGER64]=&RadiusAttributeBuilder::uint64ToString;
    converter[PW_TYPE_SIGNED] =  &RadiusAttributeBuilder::int32ToString;
    converter[PW_TYPE_BOOLEAN] = &RadiusAttributeBuilder::boolToString;
}

std::string RadiusAttributeBuilder::octetsToString(const void *attr_val, int attr_len) {
    LOGGER_IN();

    const uint8_t * attr = (uint8_t*)attr_val;
    std::stringstream stream;

    std::for_each(attr, attr+attr_len, [&stream](uint8_t c){
        stream << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (unsigned)c;
    });
    LOGGER_STR_ARG1("result = (%s)",stream.str().c_str());

    return stream.str();
}


std::string RadiusAttributeBuilder::stringToString(const void *attr_val, int attr_len) {
    LOGGER_IN();
    return std::string((const char*)attr_val);
}

std::string RadiusAttributeBuilder::uint32ToString(const void *attr_val, int attr_len) {
    LOGGER_IN();
    return std::to_string(*(uint32_t*)attr_val);
}

std::string RadiusAttributeBuilder::int32ToString(const void *attr_val, int attr_len) {
    LOGGER_IN();
    return std::to_string(*(int32_t*)attr_val);
}

std::string RadiusAttributeBuilder::ipv4ToString(const void *attr_val, int attr_len) {
    LOGGER_IN();
    std::stringstream ss;
    if (attr_len < 4) {
        return std::string("incorrect addr");
    }
    uint8_t* val = (uint8_t*)attr_val;
    ss << (uint32_t)val[0] << "." << (uint32_t)val[1] << "." << (uint32_t)val[2] << "." << (uint32_t)val[3] ;
    return ss.str();
}

std::string RadiusAttributeBuilder::dateToString(const void *attr_val, int attr_len) {
    LOGGER_IN();
    std::string str (ctime((time_t*)attr_val)); //!!! ctime is non multi-thread-safe
    if (str.length() > 0) {
        str.pop_back(); //removing end of line
    }
    return str;
}

std::string RadiusAttributeBuilder::byteToString(const void *attr_val, int attr_len) {
    LOGGER_IN();
    return std::to_string(*(uint8_t*)attr_val);
}

std::string RadiusAttributeBuilder::boolToString(const void *attr_val, int attr_len) {
    LOGGER_IN();
    return (*(uint8_t*)attr_val == 0)?std::string("False"):std::string("True");
}

std::string RadiusAttributeBuilder::ushortToString(const void *attr_val, int attr_len) {
    LOGGER_IN();
    return std::to_string(*(uint16_t*)attr_val);
}

std::string RadiusAttributeBuilder::uint64ToString(const void *attr_val, int attr_len) {
    LOGGER_IN();
    return std::to_string(*(uint64_t*)attr_val);
}

#ifdef CATCH_UNITTEST_ENABLED

TEST_CASE("RadiusAttributeBuilder", "[radius]") {
    LOGGER_IN();

    RadiusAttributeBuilder builder;
    //unsigned char buf[5] = {0,0xA, 0xFF, 0xB0, 0x1A};
    unsigned char buf[5] = {'a','b','c','l',0};
    uint32_t number = 1506062091;
    time_t time = number;

    REQUIRE("6162636C00" == builder.attrToString(buf, 5, PW_TYPE_OCTETS));
    REQUIRE("abcl"       == builder.attrToString(buf, 5, PW_TYPE_STRING));
    REQUIRE("1506062091" == builder.attrToString(&number, 4, PW_TYPE_INTEGER));
    REQUIRE("Fri Sep 22 09:34:51 2017" == builder.attrToString(&time, 4, PW_TYPE_DATE));
    number = 42345;
    REQUIRE("42345" == builder.attrToString(&number, 4, PW_TYPE_SHORT));
    number = 245;
    REQUIRE("245" == builder.attrToString(&number, 4, PW_TYPE_BYTE));
    number = 0x01020304;
    REQUIRE("4.3.2.1" == builder.attrToString(&number, 4, PW_TYPE_IPV4_ADDR));
    uint64_t lnumber = 12345678901;
    REQUIRE("12345678901" == builder.attrToString(&lnumber, 8, PW_TYPE_INTEGER64));
    int32_t snumber = -1;
    REQUIRE("-1" == builder.attrToString(&snumber, 8, PW_TYPE_SIGNED));
    buf[0] = 1;
    REQUIRE("True" == builder.attrToString(buf, 8, PW_TYPE_BOOLEAN));
    buf[0] = 0;
    REQUIRE("False" == builder.attrToString(buf, 8, PW_TYPE_BOOLEAN));
}

#endif

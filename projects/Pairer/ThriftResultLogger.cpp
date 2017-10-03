//
// Created by Andrey Khrulev on 30.09.17.
//

#include <chrono>
#include <boost/smart_ptr/make_shared.hpp>
#include "ThriftResultLogger.h"

#include "log.h"

using namespace pairer::reporting;

ThriftResultLogger::ThriftResultLogger(uint32_t app_id, const std::string &thrift_ip, int32_t thrift_port):ResultReporter(app_id) {
    socket_ = boost::shared_ptr<TSocket>(new TSocket(thrift_ip, thrift_port));
    transport_ = boost::shared_ptr<TTransport>(new TBufferedTransport(socket_));
    protocol_ = boost::shared_ptr<TProtocol>(new TBinaryProtocol(transport_));
    client_ = boost::shared_ptr<LoggerClient>(new LoggerClient(protocol_));
    transport_->open();
}

ThriftResultLogger::~ThriftResultLogger() {
    transport_->close();
}

void ThriftResultLogger::mapToAvpList(const std::map<short, std::string>& src, std::vector<RadiusAvp>& dst) {
    LOGGER_IN()
    RadiusAvp avp;
    for(auto const& avp_pair: src) {
        avp.__set_type(avp_pair.first);
        avp.__set_value(avp_pair.second);
        dst.push_back(avp);
        LOGGER_STR_ARG2("converted element '%d'/'%s'",avp.type, avp.value.c_str());
    }
}

void ThriftResultLogger::reportPair(pairer::Message *req, pairer::Message *rsp) {
    LOGGER_IN()

    LOGGER_STR_ARG1("Logging request/response id=%s", req->getKey().c_str());

    RadiusRequest request;
    mapToAvpList(req->getAttributes(),request.avpRequestList);
    request.__isset.avpRequestList = true;
    mapToAvpList(rsp->getAttributes(),request.avpResponseList);
    request.__isset.avpResponseList = true;

    using namespace std;
    using namespace std::chrono;

    request.__set_sourceId(app_id_);
    request.__set_captureTimestamp((int32_t)system_clock::to_time_t(system_clock::now()));

    client_->logRequest(request);
}

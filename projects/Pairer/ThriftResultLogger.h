//
// Created by Andrey Khrulev on 30.09.17.
//

#ifndef SAMPLEPROJECT_RESULTLOGGER_H
#define SAMPLEPROJECT_RESULTLOGGER_H

#undef PACKAGE_VERSION
#include "Logger.h"
#include "Message.h"
#include "ResultReporter.h"

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

namespace pairer {
    namespace reporting {

        /** Implementation of Reporter that sends messages to thrift server
         *
         */
        class ThriftResultLogger : public ResultReporter {

        public:
            /** constructor
             *
             * @param app_id application id
             * @param thrift_ip ip address of thrift server
             * @param thrift_port port of thrift server
             */
            ThriftResultLogger(uint32_t app_id, const std::string &thrift_ip, int32_t thrift_port);

            virtual ~ThriftResultLogger();

            /** reports of req/rsp message pairs
             *
             * @param req request message
             * @param rsp response message
             */
            void reportPair(pairer::Message *req, pairer::Message *rsp) override;

        protected:
            class ThriftServerConnectionError: public std::exception {
            public:
                const char *what() const noexcept override {
                    return "Could not connect to Thrift Server. Please ensure it is run";
                };
            };
            class ThriftServerCommunicationError: public std::exception {
            public:
                const char *what() const noexcept override {
                    return "Could not send data to Thrift Server. Please ensure it is run";
                };
            };

            /** transforms message data to thrift specific structures
             *
             * @param src message internal structure
             * @param dst thrift specific structure
             */
            void mapToAvpList(const std::map<short, std::string> &src, std::vector<RadiusAvp> &dst);

            boost::shared_ptr<TSocket> socket_;
            boost::shared_ptr<TTransport> transport_;
            boost::shared_ptr<TProtocol> protocol_;
            boost::shared_ptr<LoggerClient> client_;
        };
    }
}

#endif //SAMPLEPROJECT_RESULTLOGGER_H

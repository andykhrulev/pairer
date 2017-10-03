//
// Created by Andrey Khrulev on 02.10.17.
//

#ifndef SAMPLEPROJECT_CONFIG_H
#define SAMPLEPROJECT_CONFIG_H


#include <string>
#include <fstream>
#include <boost/program_options.hpp>
#include "log.h"

namespace pairer {

    class Config {
    private:
        const char *RECEIVER_IP = "pairer.Receiver.Ip";
        const char *RECEIVER_PORT = "pairer.Receiver.Port";
        const char *RESPONSE_TIMEOUT = "pairer.RadiusResponseTimeout";
        const char *PROXY_MODE = "pairer.ProxyModeEnabled";
        const char *STAT_PERIOD = "pairer.StatisticPeriod";

        const char *THRIFT_SRV_IP = "reporting.ThriftServer.Ip";
        const char *THRIFT_SRV_PORT = "reporting.ThriftServer.Port";

        const char *RADIUS_SRV_IP = "proxy.RadiusServer.Ip";
        const char *RADIUS_SRV_PORT = "proxy.RadiusServer.Port";

        const char *APP_ID = "AppId";
        const char *THRIFT_SRV_LOG_FNAME = "LoggingFileName";

    public:
        Config(const std::string &ini_file = std::string("pairer.ini")) {
            LOGGER_IN()

            namespace po = boost::program_options;

            try {
                std::ifstream conf_file(ini_file);

                po::variables_map vm;
                po::options_description desc("");

                desc.add_options()
                        (RECEIVER_IP, po::value<std::string>(&receiver_ip_)->default_value("127.0.0.1"))
                        (RECEIVER_PORT, po::value<uint32_t>(&receiver_port_)->default_value(9901))
                        (RESPONSE_TIMEOUT, po::value<uint32_t>( &response_timeout_)->default_value(2000))
                        (PROXY_MODE, po::value<bool>(&proxy_mode_)->default_value(false))
                        (STAT_PERIOD, po::value<uint32_t>( &stat_period_)->default_value(12000))
                        (APP_ID, po::value<uint32_t>( &app_id_)->default_value(101))
                        (THRIFT_SRV_IP, po::value<std::string>( &thrift_ip_)->default_value("127.0.0.1"))
                        (THRIFT_SRV_PORT, po::value<uint32_t>(&thrift_port_)->default_value(9090))
                        (THRIFT_SRV_LOG_FNAME, po::value<std::string>( &thrift_logging_file_name_)->default_value("thrift.log"))
                        (RADIUS_SRV_IP, po::value<std::string>( &radius_srv_ip_)->default_value( "127.0.0.1"))
                        (RADIUS_SRV_PORT, po::value<uint32_t>(&radius_srv_port_)->default_value(9801));

                po::store(po::parse_config_file(conf_file, desc, true), vm);
                po::notify(vm);
                LOGGER_STR_ARG2("receiver addr %s:%d", receiver_ip_.c_str(), receiver_port_);
            } catch (std::exception &e) {
                throw;
            }
        }

        std::string receiverIp() { return receiver_ip_; }

        uint32_t receiverPort() { return receiver_port_; }

        std::chrono::milliseconds responseTimeout() { return std::chrono::milliseconds(response_timeout_); }

        bool proxyMode() { return proxy_mode_; }

        std::chrono::milliseconds statisticPeriod() { return std::chrono::milliseconds(stat_period_); }

        std::string radiusServerIp() { return radius_srv_ip_; }

        uint32_t radiusServerPort() { return radius_srv_port_; }

        std::string thriftServerIp() { return thrift_ip_; }

        uint32_t thriftServerPort() { return thrift_port_; }

        uint32_t appId() { return app_id_; }

        std::string thriftLoggingFileName() {return thrift_logging_file_name_;};

    protected:
        std::string receiver_ip_;
        uint32_t receiver_port_;
        uint32_t response_timeout_;
        bool proxy_mode_;
        uint32_t stat_period_;
        std::string thrift_ip_;
        uint32_t thrift_port_;
        std::string radius_srv_ip_;
        uint32_t radius_srv_port_;
        uint32_t app_id_;
        std::string thrift_logging_file_name_;
    };
}

#endif //SAMPLEPROJECT_CONFIG_H


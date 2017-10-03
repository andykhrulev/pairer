//
// Created by Andrey Khrulev on 23.09.17.
//

#ifndef SAMPLEPROJECT_RADIUSPROXYMSGRECEIVER_H
#define SAMPLEPROJECT_RADIUSPROXYMSGRECEIVER_H

#include <vector>
#include "RadiusMessageReceiver.h"
namespace pairer {
    namespace radius {

        /** This is testing proxy class that adds ability for RadiusMessageReceiver
         * to proxy received request to destination after it is processed by Pairer.
         *
         * This class retrieves message from socket in peek mode and later sends it to destination.
         * As a result it may be used as transparent proxy between client and server so Pairer keeps
         * messages from both client and server.
         * Class is used for testing purposes and has the following limitations:
         * - The class is not thread-safe for simplicity of implementation
         * - Only one client and one server is supported
         */
        class RadiusProxyMsgReceiver : public RadiusMessageReceiver {
        public:
            RadiusProxyMsgReceiver() = delete;

            RadiusProxyMsgReceiver(RadiusProxyMsgReceiver &rcvr) = delete;

            RadiusProxyMsgReceiver(pairer::MessageHandler *handler, const std::string &serverIP,
                                   const uint32_t &serverPort);

            virtual ~RadiusProxyMsgReceiver();

        protected:
            /** Replaces original wait function and performs proxying packets.
             * Function sends buffered packet to destination,
             * then waits for a new packet (original behavior)
             * then buffers packet before passing control to caller
             * @param sock socket
             */
            virtual void waitForRadiusPacket(int sock);

            typedef uint8_t *uint8_ptr;

            /** Copied received packet to buffer and source addr from socket
             *  @param sock socket for reading
             *  @param buf buffer where message will be read (new memory is allocated by function)
             *  @param len length of allocated buffer, 0 if nothing read
             *  @param source ip address of sender
             */
            void copyReceivedRadiusPacket(int sock, uint8_ptr &buf, uint32_t &len, struct sockaddr &source);

            static const int NUM_RADIUS_CLIENT_ADDRS = 256;

            struct sockaddr server_addr_;
            std::vector<struct sockaddr> client_addrs_;
            uint8_t *bufferedPacket;
            uint32_t bufferedPacketLen;
            enum {
                DEST_NONE = 0, // no packet stored - looks like information duplication: TODO: optimize if required
                DEST_SERVER,   // packet to be sent to server
                DEST_CLIENT    // packet to be sent to client
            } bufferedPacketDestination;
        };
    }
}


#endif //SAMPLEPROJECT_RADIUSPROXYMSGRECEIVER_H

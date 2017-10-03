//
// Created by Andrey Khrulev on 23.09.17.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unitypes.h>
#include "RadiusProxyMsgReceiver.h"
#include "log.h"

using namespace pairer::radius;

RadiusProxyMsgReceiver::RadiusProxyMsgReceiver(pairer::MessageHandler *handler,
                                               const std::string& serverIP,
                                               const uint32_t& serverPort):
        RadiusMessageReceiver(handler), client_addrs_(NUM_RADIUS_CLIENT_ADDRS) {
    //1. initialize the server_addr_ that is address should be destination for the first packet
    memset(&(this->server_addr_), 0, sizeof(struct sockaddr)); // initialize to 0

    struct sockaddr_in* serverAddrIPv4 = (struct sockaddr_in*)(&server_addr_);

    /* Address family = Internet */
    serverAddrIPv4->sin_family = AF_INET;
    /* Set IP address from string ip */
    inet_aton(serverIP.c_str(), &(serverAddrIPv4->sin_addr));
    /* Set port number, using htons function to use proper byte order */
    serverAddrIPv4->sin_port = htons(serverPort);
    serverAddrIPv4->sin_len = sizeof(struct sockaddr_in);

    //2. initialize the clientAddr that shall be null till first packet
    for(auto addr: client_addrs_) {
        memset(&addr, 0, sizeof(struct sockaddr)); // initialize to 0
    }

    //3. initialize buffer for the packet
    this->bufferedPacket = nullptr;
    this->bufferedPacketLen = 0;
    this->bufferedPacketDestination = DEST_NONE;
}

RadiusProxyMsgReceiver::~RadiusProxyMsgReceiver(){
    if(this->bufferedPacket != nullptr) {
        free(this->bufferedPacket);
    }
}

/** Copied received packet to buffer and source addr from socket
 *  @param sock socket for reading
 *  @param buf buffer where message will be read (new memory is allocated by function)
 *  @param len length of allocated buffer, 0 if nothing read
 *  @param source ip address of sender
 */
void RadiusProxyMsgReceiver::copyReceivedRadiusPacket(int sock,
                                                      RadiusProxyMsgReceiver::uint8_ptr &buf,
                                                      uint32_t &len,
                                                      struct sockaddr &source) {
    LOGGER_IN();
    socklen_t sizeof_src = sizeof(struct sockaddr);
    char header[4];

    ssize_t data_len = recvfrom(sock, header, sizeof(header), MSG_PEEK, &source, &sizeof_src);
    if( data_len < 4) {
        // nothing is really received or radius packet corrupted, return 0
        LOGGER_STR_ARG2("Message received: read=%ld bytes, err=%s ", data_len, strerror(errno));
        len = 0;
        return;
    }
    len = (header[2] * 256) + header[3]; // read packet len from the header
    LOGGER_STR_ARG2("Message received: packet len=%d bytes, id=%d ", len, header[1]);

    buf = (uint8_t*)malloc(len);

    data_len = recvfrom(sock, buf, len, MSG_PEEK, NULL, NULL);
    if( data_len < len) {
        // radius packet corrupted, return 0
        LOGGER_STR_ARG2("Message read: read=%ld bytes, err=%s ", data_len, strerror(errno));
        len = 0;
        return;
    }
    LOGGER_STR_ADDR("Message received from", &source);
}

void RadiusProxyMsgReceiver::waitForRadiusPacket(int sock) {
    LOGGER_IN();

    // send packet if it is buffered
    if (bufferedPacketDestination != DEST_NONE) {
        // get packet identifier
        uint8_t id = bufferedPacket[1];
        struct sockaddr* dest = (bufferedPacketDestination == DEST_CLIENT)?
                                (&client_addrs_[id]):(&server_addr_);

        ssize_t sent_len = sendto(sock, bufferedPacket, bufferedPacketLen, 0, dest, dest->sa_len);
        LOGGER_STR_ADDR("Message sent to", dest);
        if (sent_len < bufferedPacketLen) {
            LOGGER_STR_ARG1(" Sending error: %s", strerror(errno));
        } else {
            LOGGER_STR_ARG1("     %lu bytes were sent ", sent_len);
        }


        bufferedPacketDestination = DEST_NONE;
        free(bufferedPacket);
        bufferedPacket = nullptr;
        bufferedPacketLen = 0;
    }

    // wait for new radius packet (original function)
    RadiusMessageReceiver::waitForRadiusPacket(sock);

    // copy radius packet from socket
    struct sockaddr srcAddr;
    uint8_t* bufPtr = nullptr;
    uint32_t bufLen = 0;

    copyReceivedRadiusPacket(sock, bufPtr, bufLen, srcAddr);

    if (bufLen <= 0) {
        LOGGER_STR("Empty message received")
        return;
    }

    if (server_addr_.sa_len == srcAddr.sa_len && memcmp(&server_addr_, &srcAddr, srcAddr.sa_len) == 0) {
        // this is message from server - it has to be sent to client
        bufferedPacketDestination = DEST_CLIENT;
    } else {
        // this is message from client
        // get id of request
        uint8_t id = bufPtr[1];

        // copy source address to the client, mark that it shall be sent to server and copy buffer
        memcpy(&client_addrs_[id], &srcAddr, srcAddr.sa_len);
        bufferedPacketDestination = DEST_SERVER;
    }
    bufferedPacket = bufPtr;
    bufferedPacketLen = bufLen;
}

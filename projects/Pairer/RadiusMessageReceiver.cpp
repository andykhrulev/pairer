//
// Created by Andrey Khrulev on 17.09.17.
//

#define RCSIDH(...)

#include <sys/socket.h>
#include <netinet/in.h>
#include <strstream>

#include "RadiusMessageReceiver.h"
#include "RadiusMessage.h"

#include "log.h"

using namespace pairer::radius;

RadiusMessageReceiver::RadiusMessageReceiver(pairer::MessageHandler *handler) : handler(handler) {

}

int RadiusMessageReceiver::createSocket() {
    LOGGER_IN();
    int sock = 0;

    struct timeval tv;
    tv.tv_sec = 3;  /* 3 Seconds Time-out */
    tv.tv_usec = 0;

    /* creating the socket */
    if (0 > (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))) {
        LOGGER_STR("Failed to create socket\n");
        throw new RadiusMessageReceiverException("Failed to create socket");
    }

    /*set the socket options*/
    //setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(struct timeval));

    return sock;
}


void RadiusMessageReceiver::bindToListeningAddress(int sock, const std::string& ip, unsigned short port) {
    LOGGER_IN();
    /*Inititalize source to zero*/
    struct sockaddr_in source;  // own address
    memset(&source, 0, sizeof(source));       //source is an instance of sockaddr_in. Initialization to zero


    /*---- Configure settings of the source address struct, WHERE THE PACKET IS COMING FROM ----*/
    /* Address family = Internet */
    source.sin_family = AF_INET;
    /* Set IP address to localhost */
    inet_aton(ip.data(), &source.sin_addr);
    /* Set port number, using htons function to use proper byte order */
    source.sin_port = htons(port);
    /* Set all bits of the padding field to 0 */
    memset(source.sin_zero, '\0', sizeof source.sin_zero); //optional


    /*bind socket to the source WHERE THE PACKET IS COMING FROM*/
    if (0 > bind(sock, (struct sockaddr *) &source, sizeof(source)))
    {
        LOGGER_STR_ARG1("Failed to bind socket: %s",strerror(errno));

        throw new RadiusMessageReceiverException("Failed to bind socket");
    }
}

void RadiusMessageReceiver::start(const std::string& ip, unsigned short port) {
    LOGGER_IN();

    int rc = dict_init("/usr/local/share/freeradius",RADIUS_DICTIONARY);
    if (rc != 0) {
        LOGGER_STR_ARG1("Dictionary could not be initialized, rc=%d",rc);
        return;
    }

    auto sock = createSocket();
    bindToListeningAddress(sock, ip, port);

    while(true) {
        try {
            waitForRadiusPacket(sock);
        }
        catch(RadiusMessageReceiverException& exception) {
            LOGGER_STR(exception.what());
            continue;
        }

        RADIUS_PACKET *packet = nullptr;
        try {
            if( nullptr == (packet = rad_recv(NULL, sock, 0))) {
                LOGGER_STR("Packet is empty - continue waiting");
                continue;
            }
            LOGGER_STR_ARG2("Packet read by radius stack (ptr=%p, len=%lu)", packet, packet->data_len);

            RADIUS_PACKET* decoded_packet = rad_alloc(NULL,0);

            rad_decode(packet, packet, NULL);


            RadiusAttributeBuilder builder;

            auto msg = new RadiusMessage(*packet, builder);

            //TODO: free packet rad_free(packet);
            rad_free(&packet);
            rad_free(&decoded_packet);

            handler->handleMessage(msg);
        }
        catch (RadiusMessageReceiverException& exception) {
            LOGGER_STR(exception.what());
            break;
        }
    }
}

void RadiusMessageReceiver::waitForRadiusPacket(int sock) {
    LOGGER_IN();
    int ret;
    fd_set  error_set;
    fd_set  read_set;

    while (true) {

        FD_SET(sock, &error_set);
        FD_SET(sock, &read_set);

        //struct timeval tv;
        //tv.tv_sec = 3;  /* 3 Seconds Time-out */
        //tv.tv_usec = 0;

        /* Don't let signals mess up the select */
        do {
            ret = select(sock + 1, &read_set, nullptr, &error_set, nullptr);
        } while ((ret == -1) && (errno == EINTR));

        switch (ret) {
            case 1: /* ok (maybe) */
            {
                if (FD_ISSET(sock, &error_set)) {
                    LOGGER_STR("Failed connecting socket: Unknown error");
                    LOGGER_STR_ARG1("Select error: %s", strerror(errno));
                    throw RadiusMessageReceiverException("Unknown error - could not get received data from socket");
                }
                if (FD_ISSET(sock, &read_set)) {
                    LOGGER_STR("Data packet received");
                    return;
                }
            }
            case -1: /* select error */
            LOGGER_STR_ARG1("Select error: %s", strerror(errno));
                throw RadiusMessageReceiverException("Unknown error - select error");
            default:
                std::strstream ss;
                ss << "Unknown error: recode =" <<ret<<std::endl;
                throw RadiusMessageReceiverException(ss.str());
        }
    }
}

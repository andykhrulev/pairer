//
// Created by Andrey Khrulev on 17.09.17.
//

#ifndef SAMPLEPROJECT_RADIUSMESSAGERECEIVER_H
#define SAMPLEPROJECT_RADIUSMESSAGERECEIVER_H

#include <string>
#include "radius.h"
#include "MessageHandler.h"
namespace pairer {
    namespace radius {

        /** Receives RADIUS message using freeradius library
         *
         */
        class RadiusMessageReceiver {
        public:
            /** constructor
             *
             * @param handler handler that will be triggered for received messages
             */
            RadiusMessageReceiver(pairer::MessageHandler *handler);

            virtual ~RadiusMessageReceiver() { delete handler; }

            /** starts main loop for receiving messages
             *
             * @param ip ip where messages are expected
             * @param port port where messages are expected
             */
            virtual void start(const std::string &ip, unsigned short port);

        protected:
            pairer::MessageHandler *handler;

            /** default exception
             *
             */
            class RadiusMessageReceiverException : public std::exception {
            public:
                RadiusMessageReceiverException(const char *err_text):err_text_(err_text) {
                }

                const char *what() const throw() {
                    return (std::string("RadiusMessageReceiverException: ") + err_text_).c_str();
                }
            private:
                const char* err_text_;
            };

            // creates socket and binds to address
            // TODO: Moves this to a separate class to make transport type (UDP/TCP) independent from the logic implementation.
            virtual int createSocket();

            // binds to provided address
            virtual void bindToListeningAddress(int sock, const std::string &ip, unsigned short port);

            // waits for new message on the socket
            virtual void waitForRadiusPacket(int sock);
        };
    };
}
#endif //SAMPLEPROJECT_RADIUSMESSAGERECEIVER_H

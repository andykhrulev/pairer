//
// Created by Andrey Khrulev on 30.09.17.
//

#ifndef SAMPLEPROJECT_RESULTREPORTER_H
#define SAMPLEPROJECT_RESULTREPORTER_H

namespace pairer {
    namespace reporting {

        class ResultReporter {
        public:
            ResultReporter(uint32_t app_id) : app_id_(app_id) {};

            virtual ~ResultReporter() {};

            virtual void reportPair(pairer::Message *req, pairer::Message *rsp) = 0;
        protected:
            uint32_t app_id_;
        };

    }
}

#endif //SAMPLEPROJECT_RESULTREPORTER_H

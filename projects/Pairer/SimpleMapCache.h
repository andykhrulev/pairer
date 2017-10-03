//
// Created by Andrey Khrulev on 24.09.17.
//

#ifndef SAMPLEPROJECT_SIMPLEMAPCACHE_H
#define SAMPLEPROJECT_SIMPLEMAPCACHE_H

#include <map>
#include "DataCache.h"

namespace pairer{
    namespace cache {

        class SimpleMapCache : public DataCache {
        public:
            SimpleMapCache();

            //SimpleMapCache(Serializer *serializer_);
            virtual ~SimpleMapCache();

            Cachable *pop_object(std::string key) override;

            void add(std::string key, Cachable *object) override;

            Cachable *first() override;

            int lookup(std::string key) override;

            class SimpleMapCacheException : public std::exception {
            public:
                SimpleMapCacheException(std::string description, std::string key, const Cachable *object) : object_(
                        object), key_(key), text_(description) {
                    std::string tmp;
                    //tmp = description;
                    text_.append(", key = ").append(key_).c_str();

                }

                const char *what() const noexcept override {
                    //return text_.append(", key = ").append(key_).c_str();
                    return text_.c_str();
                }

                virtual ~SimpleMapCacheException() {
                }

            protected:
                const Cachable *object_;
                std::string key_;
                std::string text_;
            };

        protected:
            std::map<std::string, Cachable *> map_cache_;
            std::recursive_mutex mutex_;
        };
    }
}

#endif //SAMPLEPROJECT_SIMPLEMAPCACHE_H

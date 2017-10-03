//
// Created by Andrey Khrulev on 24.09.17.
//

#ifndef SAMPLEPROJECT_DATACACHE_H
#define SAMPLEPROJECT_DATACACHE_H

#include <string>
#include <mutex>

namespace pairer{

    namespace cache {

        /** Interface/Abstract class for classes that needs to be cached
         *
         */
        class Cachable {
        protected:
            Cachable() {};
            virtual ~Cachable() {};

            Cachable(Cachable &cachable) = delete;

            /** Restores data from cache
             */
            Cachable(std::string &content) = delete;

        public:

            /** Returns string that represents object value
             *
             * @return serialized value that represents object value
             */
            virtual std::string getContent() = 0;

            /** Returns key that identifies object
             *
             * @return key that identifies object
             */
            virtual std::string getKey() = 0;

            virtual bool equal(pairer::cache::Cachable& data) {
                return (data.getKey() == getKey()) && (data.getContent() == getContent());
            }
        };

        /** Serializes and de-serializes objects to/from string
         * TODO: since not required for simple implementation - shall be moved out to db implementation for cache
         */
        class Serializer {
        protected:
            Serializer(){};

        public:
            virtual ~Serializer(){};

            /** Transforms cachable object to row data (string)
             *
             * @param obj object that is source for serialization
             * @return string that represent object content
             */
            virtual std::string getContent(Cachable *obj) = 0;

            /** Restores object from row data (string)
             *
             * @param content data content
             * @return cachable object recreated from string
             */
            virtual Cachable *getObject(std::string &content) = 0;
        };

        /** Interface for data cache.
         * Data Cache may be either local (map) or external (db) in dependence on implementation.
         * Class provides united interface
         */
        class DataCache {
        protected:
            DataCache() {
                serializer_= nullptr;
            };

            DataCache(DataCache &cache) {
            };

        public:
            DataCache(Serializer *serializer) {
                this->serializer_ = serializer;
            }
            virtual ~DataCache() {if(serializer_) delete serializer_;};

            /** lookup for object with provided key
             *
             * @param key identifies object for search
             * @return number of objects found
             */
            virtual int lookup(std::string key) = 0;

            /** add object to cache
             *
             * @param key identifies object
             * @param object object to be put to cache
             */
            virtual void add(std::string key, Cachable *object) = 0;

            /** get object and remove from cache
             *
             * @param key identifies object for lookup
             * @return object that identifies by key
             */
            virtual Cachable *pop_object(std::string key) = 0;

            /** get object that has key, that is first in cache.
             * Object is not removed from cache.
             *
             * @param lowest indicates whether lowest or highest key value will be searched for
             * @return object found
             */
            virtual Cachable *first() = 0;


        protected:
            Serializer *serializer_;
            std::mutex mutex_;
        };
    }
}


#endif //SAMPLEPROJECT_DATACACHE_H

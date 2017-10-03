//
// Created by Andrey Khrulev on 24.09.17.
//

#include "log.h"
#include "catch.hpp"
#include "SimpleMapCache.h"

pairer::cache::SimpleMapCache::SimpleMapCache() {
    map_cache_.empty();
}

int pairer::cache::SimpleMapCache::lookup(std::string key) {
    LOGGER_IN();
    std::unique_lock<std::recursive_mutex> lock(this->mutex_);
    LOGGER_STR_ARG2("Item %s is %s map", key.c_str(), (map_cache_.find(key) != map_cache_.end()?"in":"not in"));
    return map_cache_.find(key) != map_cache_.end();
}

void pairer::cache::SimpleMapCache::add(std::string key, pairer::cache::Cachable *object) {
    LOGGER_IN()
    std::unique_lock<std::recursive_mutex> lock(this->mutex_);
    LOGGER_STR_ARG2("Adding item with key = '%s' map len=%lu",key.c_str(), map_cache_.size());
    if (lookup(key) == 0) {
        //map_cache_[key] = serializer_->getContent(object);
        map_cache_[key] = object;
    } else {
        throw SimpleMapCacheException("Item with the same key does already exist in db", key, object);
    }
}

pairer::cache::Cachable *pairer::cache::SimpleMapCache::pop_object(std::string key) {
    std::unique_lock<std::recursive_mutex> lock(this->mutex_);
    auto it = map_cache_.find(key);
    if (it == map_cache_.end()) {
        return nullptr;
    }
    auto rc = it->second;
    map_cache_.erase(it);
    return rc;
}

pairer::cache::Cachable *pairer::cache::SimpleMapCache::first() {
    std::unique_lock<std::recursive_mutex> lock(this->mutex_);
    return map_cache_.empty()?nullptr: map_cache_.begin()->second;
}

pairer::cache::SimpleMapCache::~SimpleMapCache() {
}

#ifdef CATCH_UNITTEST_ENABLED

class TestCachable: public pairer::cache::Cachable {
public:
    TestCachable(const std::string& id):id_(id) {};

    std::string getKey() override {
        return id_;
    }

    std::string getContent() override {
        return id_;
    }


protected:
    std::string id_;
};

/** Set of test asserts to verify SimpleMapCache class behavior
 * The same asserts can be used to verify any DataCache ancestor for compatibility with DataCache interface
 */
TEST_CASE("SimpleMapCache", "[cache]") {
    LOGGER_IN()

    pairer::cache::DataCache * cache;

    // 1. object creation
    REQUIRE_NOTHROW(cache = new pairer::cache::SimpleMapCache());

    TestCachable tdata1("12"), tdata2("13"), tdata3("124"), tdata4(""), tdata5("$$$$");

    // 1. empty cache - verify lookup, front, get
    REQUIRE( 0 == cache->lookup(tdata1.getKey()));
    REQUIRE(cache->first() == nullptr);
    REQUIRE( 0 == cache->pop_object(tdata1.getKey()));

    // 2. add data
    REQUIRE_NOTHROW(cache->add(tdata1.getKey(), &tdata1));

    // 3. added data present
    REQUIRE(1 == cache->lookup(tdata1.getKey()));

    // 4. add more data
    REQUIRE_NOTHROW(cache->add(tdata2.getKey(), &tdata2));
    REQUIRE_NOTHROW(cache->add(tdata4.getKey(), &tdata4));
    REQUIRE_NOTHROW(cache->add(tdata5.getKey(), &tdata5));

    //4.1. add data that is already in cache - no doublicates accepted
    REQUIRE_THROWS(cache->add(tdata5.getKey(), &tdata5));

    // 5. lookup for added data
    REQUIRE(1 == cache->lookup(tdata2.getKey()));
    REQUIRE(1 == cache->lookup(tdata4.getKey()));
    REQUIRE(1 == cache->lookup(tdata5.getKey()));
    REQUIRE(1 == cache->lookup(tdata1.getKey()));
    // 6. lookup for absent data
    REQUIRE(0 == cache->lookup(tdata3.getKey()));

    // 7. verify first
    REQUIRE(cache->first() != nullptr);

    LOGGER_STR_ARG1("First %s", cache->first()->getKey().c_str())
    REQUIRE(cache->first()->equal(tdata4));
    REQUIRE(cache->first() == &tdata4);

    // 8. verify retrieve data on non-empty cache
    // 8.1. retrieve absent object
    REQUIRE(cache->pop_object(tdata3.getKey()) == nullptr);
    // 8.2. retrieve existed object
    REQUIRE(cache->pop_object(tdata2.getKey())->equal(tdata2));
    REQUIRE(0 == cache->lookup(tdata3.getKey()));
    REQUIRE(cache->pop_object(tdata2.getKey()) == nullptr);
    // 8.3. retrieve first object
    pairer::cache::Cachable* first = cache->first();
    REQUIRE(cache->pop_object(first->getKey()) == first);
    REQUIRE(0 == cache->lookup(first->getKey()));

    REQUIRE_NOTHROW(delete cache);
}
#endif

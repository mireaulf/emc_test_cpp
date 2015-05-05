
#include <string>
using std::string;

#include "../include/common.h"
#include "../../include/EMC/lookup_buffer.h"
using emc::lookup_buffer;
#include "../../include/EMC/exceptions.h"

static const string SCENARIO_NAME("emc::lookup_buffer");

SCENARIO("a lookup buffer can be constructed/destroyed", SCENARIO_NAME.data()) {
    const unsigned short BUFFER_LEN = 35;
    GIVEN("an empty string") {
        const string lookup("");
        THEN("the construction fails") {
            REQUIRE_THROWS_AS(lookup_buffer<BUFFER_LEN> buf(lookup), emc::exceptions::lookup_buffer::empty_string);
        }
    }
    GIVEN("a string too long") {
        const string lookup("this string is too loooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooong");
        THEN("the construction fails") {
            REQUIRE_THROWS_AS(lookup_buffer<BUFFER_LEN>  buf(lookup), emc::exceptions::lookup_buffer::too_long_string);
        }
    }
    GIVEN("a string long enough, but not too long") {
        const string lookup("hello World!");
        THEN("the construction succeeds") {
            REQUIRE_NOTHROW(lookup_buffer<BUFFER_LEN> buf(lookup));
            lookup_buffer<BUFFER_LEN> buf(lookup);
            const string str = buf.get_lookup();
            REQUIRE(str == lookup);
            const auto str_len = str.length();
            const auto free_space = buf.get_free_space_buffer();
            REQUIRE(str_len == BUFFER_LEN - free_space);
        }
    }
}

SCENARIO("a lookup buffer is used to search content", SCENARIO_NAME.data()) {
    GIVEN("a lookup buffer properly initialized") {
        lookup_buffer<50> buf("hello world");
        WHEN("searching in an empty string") {
            THEN("the search fails") {
                REQUIRE_THROWS_AS(buf.search_in(""), emc::exceptions::lookup_buffer::empty_string);
            }
        }
        WHEN("searching in a non empty string") {
            GIVEN("the content string too long for the lookup buffer") {
                THEN("the search fails") {
                    REQUIRE_THROWS_AS(
                        buf.search_in("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaahello worldbbb"), 
                        emc::exceptions::lookup_buffer::too_long_string);
                }
            }
            GIVEN("the lookup string is not in the search content") {
                THEN("the search fails") {
                    REQUIRE_FALSE(buf.search_in("aaaaaaaaaaaaaaaa qaaaaaa"));
                    REQUIRE_THROWS_AS(buf.get_next_word(), emc::exceptions::lookup_buffer::not_found);
                }
            }
            GIVEN("the lookup string is in the content") {
                THEN("the search succeeds") {
                    AND_WHEN("search in a single buffer") {
                        REQUIRE(buf.search_in("asd hello worlddd1d"));
                        REQUIRE(buf.get_next_word() == "dd1d");
                    }
                    AND_WHEN("search in multiple buffer") {
                        REQUIRE_FALSE(buf.search_in("asd hello "));
                        REQUIRE_FALSE(buf.search_in("wasd hello "));
                        REQUIRE(buf.search_in("worldd2dd"));
                        REQUIRE(buf.get_next_word() == "d2dd");
                    }
                }
            }
        }        
    }
}

#include "vertex/_test/test.hpp"

#include "vertex/util/string/regex.hpp"

using namespace vx;

VX_TEST_CASE(regex)
{
    VX_SECTION("validation")
    {
        re::regex valid_regex("^[a-z]+$");
        VX_CHECK(valid_regex.is_valid());

        re::regex invalid_regex("[a-z");
        VX_CHECK(!invalid_regex.is_valid());
    }

    VX_SECTION("match")
    {
        re::regex r("^hello$");

        re::regex_cmatch match;
        VX_CHECK(r.match("hello", match));
        VX_CHECK(match.size() == 1);
        VX_CHECK(std::string(match[0].first, match[0].second) == "hello");

        VX_CHECK(!r.match("world", match));
    }

    VX_SECTION("search")
    {
        re::regex r("world");

        re::regex_cmatch match;
        VX_CHECK(r.search("hello world", match));
        VX_CHECK(match.size() == 1);
        VX_CHECK(std::string(match[0].first, match[0].second) == "world");

        VX_CHECK(!r.search("hello", match));
    }

    VX_SECTION("search all")
    {
        re::regex r("(\\b\\w+\\b)"); // Matches words (sequences of word characters between word boundaries)

        auto matches = r.search_all("Hello, world! This is a test sentence.");
        VX_CHECK(matches.size() == 7); // Expected: 7 words

        VX_CHECK(std::string(matches[0][0].first, matches[0][0].second) == "Hello");
        VX_CHECK(std::string(matches[1][0].first, matches[1][0].second) == "world");
        VX_CHECK(std::string(matches[2][0].first, matches[2][0].second) == "This");
        VX_CHECK(std::string(matches[3][0].first, matches[3][0].second) == "is");
        VX_CHECK(std::string(matches[4][0].first, matches[4][0].second) == "a");
        VX_CHECK(std::string(matches[5][0].first, matches[5][0].second) == "test");
        VX_CHECK(std::string(matches[6][0].first, matches[6][0].second) == "sentence");
    }

    VX_SECTION("replace")
    {
        re::regex r("world");

        auto replaced = r.replace("hello world", "universe");
        VX_CHECK(replaced == "hello universe");

        auto no_match = r.replace("hello there", "universe");
        VX_CHECK(no_match == "hello there");
    }

    VX_SECTION("iteration")
    {
        re::regex r("(\\d+)-(\\d+)");
        VX_CHECK(r.group_count() == 2);

        const char* input = "The events are scheduled for 10-20, 30-40, and 50-60.";
        auto it = r.begin(input, input + std::strlen(input));
        auto end = r.end(input);

        std::vector<std::string> results;
        for (; it != end; ++it)
        {
            // Group 0: Full match, Group 1: First number, Group 2: Second number
            results.push_back(it->str(0)); // Full match
            results.push_back(it->str(1)); // First group
            results.push_back(it->str(2)); // Second group
        }

        // Check number of matches
        VX_CHECK(results.size() == 9); // 3 matches * 3 groups

        // Check the first match
        VX_CHECK(results[0] == "10-20");
        VX_CHECK(results[1] == "10");
        VX_CHECK(results[2] == "20");

        // Check the second match
        VX_CHECK(results[3] == "30-40");
        VX_CHECK(results[4] == "30");
        VX_CHECK(results[5] == "40");

        // Check the third match
        VX_CHECK(results[6] == "50-60");
        VX_CHECK(results[7] == "50");
        VX_CHECK(results[8] == "60");
    }

    VX_SECTION("groups")
    {
        re::regex r("(\\b\\w+\\b)\\s(\\b\\w+\\b)"); // Matches two consecutive words
        VX_CHECK(r.group_count() == 2);

        auto matches = r.search_all("hello world, this is a test.");
        VX_CHECK(matches.size() == 3);

        // Verify each match
        VX_CHECK(std::string(matches[0][0].first, matches[0][0].second) == "hello world");
        VX_CHECK(std::string(matches[1][0].first, matches[1][0].second) == "this is");
        VX_CHECK(std::string(matches[2][0].first, matches[2][0].second) == "a test");

        VX_CHECK(std::string(matches[0][1].first, matches[0][1].second) == "hello");
        VX_CHECK(std::string(matches[0][2].first, matches[0][2].second) == "world");
        VX_CHECK(std::string(matches[1][1].first, matches[1][1].second) == "this");
        VX_CHECK(std::string(matches[1][2].first, matches[1][2].second) == "is");
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}
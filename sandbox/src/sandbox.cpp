#include "sandbox/sandbox.hpp"

#include "vertex/util/string/string_compare.hpp"

using namespace vx;

static void test_levenshtein_distance()
{
    // Test 1: Identical strings (expected distance = 0)
    {
        std::string str1 = "hello";
        std::string str2 = "hello";
        assert(vx::str::levenshtein_distance(str1.begin(), str1.end(), str2.begin(), str2.end()) == 0);
    }

    // Test 2: Single substitution (expected distance = 1)
    {
        std::string str1 = "hello";
        std::string str2 = "hxllo";
        assert(vx::str::levenshtein_distance(str1.begin(), str1.end(), str2.begin(), str2.end()) == 1);
    }

    // Test 3: Insertion (expected distance = 4)
    {
        std::string str1 = "kitten";
        std::string str2 = "sitting";
        assert(vx::str::levenshtein_distance(str1.begin(), str1.end(), str2.begin(), str2.end()) == 3); // insertion, substitution, insertion
    }

    // Test 4: Empty string (expected distance = 5)
    {
        std::string str1 = "kitten";
        std::string str2 = "";
        assert(vx::str::levenshtein_distance(str1.begin(), str1.end(), str2.begin(), str2.end()) == 6); // all characters must be deleted
    }
}

static void test_damerau_levenshtein_distance()
{
    // Test 1: Identical strings (expected distance = 0)
    {
        std::string str1 = "hello";
        std::string str2 = "hello";
        assert(vx::str::damerau_levenshtein_distance(str1.begin(), str1.end(), str2.begin(), str2.end()) == 0);
    }

    // Test 2: Single substitution (expected distance = 1)
    {
        std::string str1 = "hello";
        std::string str2 = "hxllo";
        assert(vx::str::damerau_levenshtein_distance(str1.begin(), str1.end(), str2.begin(), str2.end()) == 1);
    }

    // Test 3: Insertion (expected distance = 3)
    {
        std::string str1 = "kitten";
        std::string str2 = "sitting";
        assert(vx::str::damerau_levenshtein_distance(str1.begin(), str1.end(), str2.begin(), str2.end()) == 3); // insertion, substitution, insertion
    }

    // Test 4: Transposition (expected distance = 2 for Damerau-Levenshtein)
    {
        std::string str1 = "ab";
        std::string str2 = "ba";
        assert(vx::str::damerau_levenshtein_distance(str1.begin(), str1.end(), str2.begin(), str2.end()) == 1); // Transposition (Damerau only)
    }

    // Test 5: Empty string (expected distance = 6 for "kitten" to "")
    {
        std::string str1 = "kitten";
        std::string str2 = "";
        assert(vx::str::damerau_levenshtein_distance(str1.begin(), str1.end(), str2.begin(), str2.end()) == 6); // all characters must be deleted
    }
}

int main(int argc, char* argv[])
{
    test_levenshtein_distance();
    test_damerau_levenshtein_distance();

    return 0;
}
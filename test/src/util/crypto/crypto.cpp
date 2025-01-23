#include "vertex/_test/test.hpp"

#include "vertex/util/crypto/MD5.hpp"
#include "vertex/util/crypto/SHA1.hpp"
#include "vertex/util/crypto/SHA256.hpp"

///////////////////////////////////////////////////////////////////////////////
// MD5
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(MD5)
{
    VX_SECTION("initial state")
    {
        vx::crypto::MD5 md5;
        md5.finalize(); // Finalize without any input
        VX_CHECK(md5.to_string() == "d41d8cd98f00b204e9800998ecf8427e");
    }

    VX_SECTION("hash simple input")
    {
        vx::crypto::MD5 md5;
        const uint8_t data[] = "abc";
        md5.update(data, sizeof(data) - 1); // Exclude null terminator
        md5.finalize();
        VX_CHECK(md5.to_string() == "900150983cd24fb0d6963f7d28e17f72");
    }

    VX_SECTION("hash long input")
    {
        vx::crypto::MD5 md5;
        const uint8_t data[] = "The quick brown fox jumps over the lazy dog";
        md5.update(data, sizeof(data) - 1);
        md5.finalize();
        VX_CHECK(md5.to_string() == "9e107d9d372bb6826bd81d3542a419d6");
    }

    VX_SECTION("hash large input")
    {
        vx::crypto::MD5 md5;
        std::string large_input(1000, 'a'); // Create a string of 1000 'a' characters
        md5.update(reinterpret_cast<const uint8_t*>(large_input.data()), large_input.size());
        md5.finalize();
        VX_CHECK(md5.to_string() == "cabe45dcc9ae5b66ba86600cca6b8ba8");
    }
}

///////////////////////////////////////////////////////////////////////////////
// SHA1
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(SHA1)
{
    VX_SECTION("initial state")
    {
        vx::crypto::SHA1 sha1;
        sha1.finalize(); // Finalize without any input
        VX_CHECK(sha1.to_string() == "da39a3ee5e6b4b0d3255bfef95601890afd80709");
    }

    VX_SECTION("hash simple input")
    {
        vx::crypto::SHA1 sha1;
        const uint8_t data[] = "abc";
        sha1.update(data, sizeof(data) - 1); // Exclude null terminator
        sha1.finalize();
        VX_CHECK(sha1.to_string() == "a9993e364706816aba3e25717850c26c9cd0d89d");
    }

    VX_SECTION("hash long input")
    {
        vx::crypto::SHA1 sha1;
        const uint8_t data[] = "The quick brown fox jumps over the lazy dog";
        sha1.update(data, sizeof(data) - 1);
        sha1.finalize();
        VX_CHECK(sha1.to_string() == "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12");
    }

    VX_SECTION("hash large input")
    {
        vx::crypto::SHA1 sha1;
        std::string large_input(1000, 'a'); // Create a string of 1000 'a' characters
        sha1.update(reinterpret_cast<const uint8_t*>(large_input.data()), large_input.size());
        sha1.finalize();
        VX_CHECK(sha1.to_string() == "291e9a6c66994949b57ba5e650361e98fc36b1ba");
    }
}

///////////////////////////////////////////////////////////////////////////////
// SHA256
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(SHA256)
{
    VX_SECTION("initial state")
    {
        vx::crypto::SHA256 sha256;
        sha256.finalize(); // Finalize without any input
        VX_CHECK(sha256.to_string() == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    }

    VX_SECTION("hash simple input")
    {
        vx::crypto::SHA256 sha256;
        const uint8_t data[] = "abc";
        sha256.update(data, sizeof(data) - 1); // Exclude null terminator
        sha256.finalize();
        VX_CHECK(sha256.to_string() == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    }

    VX_SECTION("hash long input")
    {
        vx::crypto::SHA256 sha256;
        const uint8_t data[] = "The quick brown fox jumps over the lazy dog";
        sha256.update(data, sizeof(data) - 1);
        sha256.finalize();
        VX_CHECK(sha256.to_string() == "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
    }

    VX_SECTION("hash large input")
    {
        vx::crypto::SHA256 sha256;
        std::string large_input(1000, 'a'); // Create a string of 1000 'a' characters
        sha256.update(reinterpret_cast<const uint8_t*>(large_input.data()), large_input.size());
        sha256.finalize();
        VX_CHECK(sha256.to_string() == "41edece42d63e8d9bf515a9ba6932e1c20cbc9f5a5d134645adb5db1b9737ea3");
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}
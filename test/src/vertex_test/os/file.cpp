#include "vertex_test/test.hpp"
#include "vertex/os/file.hpp"
#include "vertex/os/filesystem.hpp" // only for temp path
#include "vertex/util/memory.hpp"

using namespace vx;

static std::string current_time_file()
{
    const time::time_point now = os::system_time();
    std::string filename = std::to_string(now.as_nanoseconds());
    filename.append(".txt");
    return filename;
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(file)
{
    //VX_PRINT_ERRORS(true);

    const os::path temp_path = os::filesystem::get_temp_path();
    const os::path filename = temp_path / current_time_file();

    const char in_text[] = "Hello World!";
    constexpr size_t count = sizeof(in_text) - 1;
    char out_text[count]{};

    VX_SECTION("file creation")
    {
        VX_CHECK(!os::file::exists(filename));
        VX_CHECK(os::file::create(filename));
        VX_CHECK(os::file::exists(filename));
    }

    VX_SECTION("open and close")
    {
        os::file f;

        VX_CHECK(!f.is_open());
        VX_CHECK(f.get_mode() == os::file::mode::NONE);
        VX_CHECK(!f.can_read());
        VX_CHECK(!f.can_write());

        VX_CHECK(f.size() == 0);

        VX_CHECK(!f.seek(0));
        VX_CHECK(f.tell() == os::file::INVALID_POSITION);
        VX_CHECK(!f.eof());

        uint8_t byte{};
        VX_CHECK(!f.read(&byte, 1));
        VX_CHECK(!f.write(&byte, 1));
        VX_CHECK(!f.flush());

        VX_CHECK(f.open(filename, os::file::mode::READ_WRITE_EXISTS));
        VX_CHECK(f.is_open());

        // File is already open, should fail
        VX_CHECK(!f.open(filename, os::file::mode::READ));

        f.close();
        VX_CHECK(!f.is_open());
        VX_CHECK(f.get_mode() == os::file::mode::NONE);
    }

    VX_SECTION("seek and tell")
    {
        os::file f;
        VX_CHECK(f.open(filename, os::file::mode::READ));
        VX_CHECK(f.is_open());

        VX_CHECK(f.tell() == 0);
        VX_CHECK(f.seek(0, os::stream_position::END));
        VX_CHECK(f.eof());

        VX_CHECK(f.seek(1000000));
        VX_CHECK(f.tell() == 1000000);

        VX_CHECK(f.seek(-5, os::stream_position::CURRENT));
        VX_CHECK(f.tell() == 1000000 - 5);

        // Cannot move pointer before the beginning
        VX_CHECK(!f.seek(-1000000));
    }

    VX_SECTION("mode: NONE")
    {
        os::file f;
        VX_CHECK(!f.open(temp_path / "test_file.txt", os::file::mode::NONE));
        VX_CHECK(!f.is_open());
    }

    // Read from and write to the file we created initially
    VX_SECTION("mode: READ_WRITE_EXISTS")
    {
        os::file f;

        // First try to open a file that does not exist (should fail)
        VX_CHECK(!f.open(temp_path / "fakefile", os::file::mode::READ_WRITE_EXISTS));
        // Open the file we created initially
        VX_CHECK(f.open(filename, os::file::mode::READ_WRITE_EXISTS));
        VX_CHECK(f.is_open());

        VX_CHECK(f.get_mode() == os::file::mode::READ_WRITE_EXISTS);
        VX_CHECK(f.can_read());
        VX_CHECK(f.can_write());

        // Write to the file
        VX_CHECK(f.write(in_text, count) == count);
        VX_CHECK(f.size() == count);

        // Read from the file
        VX_CHECK(f.seek(0));
        VX_CHECK(f.read(out_text, count) == count);

        // The text written should match the text read
        VX_CHECK(std::memcmp(in_text, out_text, count) == 0);
        VX_CHECK(f.eof()); // Should be at the end of the file

        // Reopening the file should not truncate the text
        f.close();
        VX_CHECK(f.open(filename, os::file::mode::READ_WRITE_EXISTS));
        VX_CHECK(f.size() == count);
        VX_CHECK(f.read(out_text, count) == count);
        VX_CHECK(std::memcmp(in_text, out_text, count) == 0);
    }

    // Read from and write to the file we created initially
    VX_SECTION("mode: READ_WRITE_CREATE")
    {
        os::file f;

        // Open the file we created initially
        VX_CHECK(f.open(filename, os::file::mode::READ_WRITE_CREATE));
        VX_CHECK(f.is_open());

        VX_CHECK(f.get_mode() == os::file::mode::READ_WRITE_CREATE);
        VX_CHECK(f.can_read());
        VX_CHECK(f.can_write());

        // File should be truncated
        VX_CHECK(f.size() == 0);

        // Write to the file
        VX_CHECK(f.write(in_text, count));
        VX_CHECK(f.size() == count);

        // Read from the file
        VX_CHECK(f.seek(0));
        VX_CHECK(f.read(out_text, count) == count);

        // The text written should match the text read
        VX_CHECK(std::memcmp(in_text, out_text, count) == 0);
        VX_CHECK(f.eof()); // Should be at the end of the file
        f.close();

        // Try opening a new file, it should be created
        const os::path new_filename = temp_path / current_time_file();
        VX_CHECK(f.open(new_filename, os::file::mode::READ_WRITE_CREATE));
        VX_CHECK(f.is_open());
        VX_CHECK(os::file::exists(new_filename));
    }

    VX_SECTION("mode: READ")
    {
        os::file f;

        // Open the file we created initially
        VX_CHECK(f.open(filename, os::file::mode::READ));
        VX_CHECK(f.is_open());

        VX_CHECK(f.get_mode() == os::file::mode::READ);
        VX_CHECK(f.can_read());
        VX_CHECK(!f.can_write());

        // File should contain "Hello World!" from the last test
        const size_t size = f.size();
        VX_CHECK(size != 0);

        // Try to write to the file
        VX_CHECK(!f.write(in_text, count));
        VX_CHECK(f.size() == size); // Size should not have changed

        // Read from the file
        VX_CHECK(f.seek(0));
        VX_CHECK(f.read(out_text, count) == count);

        // The text written should match the text read
        VX_CHECK(std::memcmp(in_text, out_text, count) == 0);
        VX_CHECK(f.eof()); // Should be at the end of the file
        f.close();
    }

    VX_SECTION("mode: WRITE")
    {
        os::file f;

        // Open the file we created initially
        VX_CHECK(f.open(filename, os::file::mode::WRITE));
        VX_CHECK(f.is_open());

        VX_CHECK(f.get_mode() == os::file::mode::WRITE);
        VX_CHECK(!f.can_read());
        VX_CHECK(f.can_write());

        // File should be truncated
        VX_CHECK(f.size() == 0);

        // Try to read from the file
        VX_CHECK(!f.read(out_text, count));

        // Write to the file
        VX_CHECK(f.write(in_text, count) == count);
        VX_CHECK(f.size() == count);

        f.close();

        // Try opening a new file, it should be created
        const os::path new_filename = temp_path / current_time_file();
        VX_CHECK(f.open(new_filename, os::file::mode::WRITE));
        VX_CHECK(f.is_open());
        VX_CHECK(os::file::exists(new_filename));
    }

    VX_SECTION("mode: APPEND")
    {
        os::file f;

        // Open the file we created initially
        VX_CHECK(f.open(filename, os::file::mode::APPEND));
        VX_CHECK(f.is_open());

        VX_CHECK(f.get_mode() == os::file::mode::APPEND);
        VX_CHECK(!f.can_read());
        VX_CHECK(f.can_write());

        // File should not be truncated
        VX_CHECK(f.size() == count);

        // Try to read from the file
        VX_CHECK(!f.read(out_text, count));

        // Seeking should still work
        VX_CHECK(f.seek(0));
        VX_CHECK(f.tell() == 0);

        // All writes should reposition the pointer to the end
        VX_CHECK(f.write(in_text, count) == count);
        VX_CHECK(f.tell() == count * 2);

        f.close();
        
        // Try opening a new file, it should be created
        const os::path new_filename = temp_path / current_time_file();
        VX_CHECK(f.open(new_filename, os::file::mode::APPEND));
        VX_CHECK(f.is_open());
        VX_CHECK(os::file::exists(new_filename));
    }

    VX_SECTION("read and write data")
    {
        os::file f;

        VX_CHECK(f.open(filename, os::file::mode::READ_WRITE_CREATE));
        VX_CHECK(f.is_open());

        const int in_1[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        const size_t in_1_count = mem::array_size(in_1);
        const float in_2 = 1.23456f;
        const time::datetime in_3 = os::system_time().to_datetime();

        VX_CHECK(f.write(in_1, in_1_count) == sizeof(in_1));
        VX_CHECK(f.write(in_2) == sizeof(in_2));
        VX_CHECK(f.write(in_3) == sizeof(in_3));

        VX_CHECK(f.seek(0));
        int out_1[in_1_count]{};
        float out_2{};
        time::datetime out_3{};

        VX_CHECK(f.read(out_1, in_1_count) == sizeof(in_1));
        VX_CHECK(f.read(out_2) == sizeof(out_2));
        VX_CHECK(f.read(out_3) == sizeof(out_3));
        VX_CHECK(f.eof());

        VX_CHECK(std::memcmp(in_1, out_1, sizeof(in_1)) == 0);
        VX_CHECK(in_2 == out_2);
        VX_CHECK(in_3 == out_3);
    }

    VX_SECTION("flush")
    {
        os::file f;

        // Open the file and truncate
        VX_CHECK(f.open(filename, os::file::mode::READ_WRITE_CREATE));
        VX_CHECK(f.is_open());

        // Write some data to the file
        VX_CHECK(f.write(in_text, count) == count);

        // Flush the file to ensure data is written to disk
        VX_CHECK(f.flush());

        // Verify that the data is still intact after flushing
        VX_CHECK(f.seek(0));
        VX_CHECK(f.read(out_text, count) == count);
        VX_CHECK(std::memcmp(in_text, out_text, count) == 0);
    }

    VX_SECTION("resize")
    {
        os::file f;

        // Open the file and truncate
        VX_CHECK(f.open(filename, os::file::mode::READ_WRITE_CREATE));
        VX_CHECK(f.is_open());

        // Write some data to the file
        VX_CHECK(f.write(in_text, count) == count);
        VX_CHECK(f.size() == count);

        // Resize the file to a larger size
        const size_t new_size_larger = count * 2;
        VX_CHECK(f.resize(new_size_larger));
        VX_CHECK(f.size() == new_size_larger);

        // Verify that the original data is still intact
        VX_CHECK(f.seek(0));
        VX_CHECK(f.read(out_text, count) == count);
        VX_CHECK(std::memcmp(in_text, out_text, count) == 0);

        // Resize the file to a smaller size
        const size_t new_size_smaller = count / 2;
        VX_CHECK(f.resize(new_size_smaller));
        VX_CHECK(f.size() == new_size_smaller);

        // Verify that the data is truncated
        VX_CHECK(f.seek(0));
        VX_CHECK(f.read(out_text, new_size_smaller) == new_size_smaller);
        VX_CHECK(std::memcmp(in_text, out_text, new_size_smaller) == 0);
    }

    VX_SECTION("static read and write file")
    {
        const char* text =
            "Hello, this is a test file for the Vertex c++ library. "
            "At the time I am writing this I am just finishing up testing "
            "my file module. I am pretty proud of it. Next, I will finish "
            "testing the remaining modules in the os section of my library, "
            "then I will move on to finishing the math section. It is "
            "February 2, 2025. I am quite nervous about the future of our "
            "country... Never the less, I continue on my journey.";

        const uint8_t* data = reinterpret_cast<const uint8_t*>(text);
        const size_t size = std::strlen(text);
        std::vector<uint8_t> out_data;

        const os::path new_filename = temp_path / current_time_file();
        VX_CHECK(os::file::write_file(new_filename, data, size));
        VX_CHECK(os::file::read_file(new_filename, out_data));
        VX_CHECK(out_data.size() == size);
        VX_CHECK(std::memcmp(text, out_data.data(), size) == 0);
    }
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}
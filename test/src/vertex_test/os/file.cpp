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

static bool read_and_compare_text_file(const os::path& p, const char* expected)
{
    const size_t expected_size = std::strlen(expected);

    std::string text;
    if (!os::file::read_file(p, text))
    {
        return false;
    }

    return std::equal(text.begin(), text.end(), expected, expected + expected_size);
}

///////////////////////////////////////////////////////////////////////////////

static const os::path temp_path = os::filesystem::get_temp_path();
static const os::path filename = temp_path / current_time_file();

static const char in_text[] = "Hello World!";
static constexpr size_t count = sizeof(in_text) - 1;

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_file_creation)
{
    VX_CHECK(!os::file::exists(filename));
    VX_CHECK(os::file::create(filename));
    VX_CHECK(os::file::exists(filename));
}

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_open_and_close)
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

    VX_CHECK(f.open(filename, os::file::mode::READ_WRITE_CREATE));
    VX_CHECK(f.is_open());

    // File is already open, should fail
    VX_CHECK_AND_EXPECT_ERROR(!f.open(filename, os::file::mode::READ));

    f.close();
    VX_CHECK(!f.is_open());
    VX_CHECK(f.get_mode() == os::file::mode::NONE);
}

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_move_operators)
{
    os::file f1;
    VX_CHECK(f1.open(filename, os::file::mode::READ));
    VX_CHECK(f1.is_open());

    VX_DISABLE_MSVC_WARNING_PUSH();
    VX_DISABLE_MSVC_WARNING(26800); // disable use after move warning

    // Move construction
    os::file f2(std::move(f1));
    VX_CHECK(!f1.is_open()); // Original file should be closed
    VX_CHECK(f2.is_open());

    // Move assignment
    os::file f3;
    f3 = std::move(f2);
    VX_CHECK(!f2.is_open()); // Moved-from file should be closed
    VX_CHECK(f3.is_open());
}

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_seek_and_tell)
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
    VX_CHECK_AND_EXPECT_ERROR(!f.seek(-1000000));
}

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mode_NONE)
{
    os::file f;
    VX_CHECK(!f.open(temp_path / "test_file.txt", os::file::mode::NONE));
    VX_CHECK(!f.is_open());
}

/////////////////////////////////////////////////////////////////////////////

// Read from and write to the file we created initially
VX_TEST_CASE(test_mode_READ_WRITE_EXISTS)
{
    char out_text[count]{};
    os::file f;

    // First try to open a file that does not exist (should fail)
    VX_CHECK_AND_EXPECT_ERROR(!f.open(temp_path / "fakefile", os::file::mode::READ_WRITE_EXISTS));
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

/////////////////////////////////////////////////////////////////////////////

// Read from and write to the file we created initially
VX_TEST_CASE(test_mode_READ_WRITE_CREATE)
{
    char out_text[count]{};
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

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mode_READ)
{
    char out_text[count]{};
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
    VX_CHECK_AND_EXPECT_ERROR(!f.write(in_text, count));
    VX_CHECK(f.size() == size); // Size should not have changed

    // Read from the file
    VX_CHECK(f.seek(0));
    VX_CHECK(f.read(out_text, count) == count);

    // The text written should match the text read
    VX_CHECK(std::memcmp(in_text, out_text, count) == 0);
    VX_CHECK(f.eof()); // Should be at the end of the file
    f.close();
}

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mode_WRITE)
{
    char out_text[count]{};
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
    VX_CHECK_AND_EXPECT_ERROR(!f.read(out_text, count));

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

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mode_APPEND)
{
    char out_text[count]{};
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
    VX_CHECK_AND_EXPECT_ERROR(!f.read(out_text, count));

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

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_read_and_write_data)
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

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_flush)
{
    char out_text[count]{};
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

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_resize)
{
    char out_text[count]{};
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

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_static_read_and_write_file)
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

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_static_read_and_write_text_file)
{
    const char* text =
        "Hello, this is a test file for the Vertex c++ library.\n"
        "At the time I am writing this I am just finishing up testing my file module.\n"
        "I am pretty proud of it.\n"
        "Next, I will finish testing the remaining modules in the os section of my library, then I will move on to finishing the math section.\n"
        "It is February 2, 2025.\n"
        "I am quite nervous about the future of our country...\n"
        "Never the less, I continue on my journey.";

    const os::path new_filename = temp_path / current_time_file();
    VX_CHECK(os::file::write_file(new_filename, text));

    const char* expected_text =
        "Hello, this is a test file for the Vertex c++ library." VX_LINE_END
        "At the time I am writing this I am just finishing up testing my file module." VX_LINE_END
        "I am pretty proud of it." VX_LINE_END
        "Next, I will finish testing the remaining modules in the os section of my library, then I will move on to finishing the math section." VX_LINE_END
        "It is February 2, 2025." VX_LINE_END
        "I am quite nervous about the future of our country..." VX_LINE_END
        "Never the less, I continue on my journey.";

    VX_CHECK(read_and_compare_text_file(new_filename, expected_text));
}

/////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_read_and_write_line)
{
    os::file f;

    // open the file and truncate
    VX_CHECK(f.open(filename, os::file::mode::WRITE));
    VX_CHECK(f.is_open());

    // write our lines
    VX_CHECK(f.write_line("Hello World"));
    VX_CHECK(f.write_line("this is the second line"));
    VX_CHECK(f.write_line(""));
    VX_CHECK(f.write_line("this is the end"));
    f.close();

    // check that the written text is correct
    const char* expected_text =
        "Hello World"               VX_LINE_END
        "this is the second line"   VX_LINE_END
        ""                          VX_LINE_END
        "this is the end"           VX_LINE_END;

    VX_CHECK(read_and_compare_text_file(filename, expected_text));

    // open file for reading
    VX_CHECK(f.open(filename, os::file::mode::READ));
    VX_CHECK(f.is_open());

    std::string line;
    VX_CHECK(f.read_line(line) && line == "Hello World");
    VX_CHECK(f.read_line(line) && line == "this is the second line");
    VX_CHECK(f.read_line(line) && line == "");
    VX_CHECK(f.read_line(line) && line == "this is the end");
    VX_CHECK(f.eof());
}

/////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}
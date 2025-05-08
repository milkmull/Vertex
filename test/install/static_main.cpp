#include <vertex/util/io/iostream.hpp>
#include <vertex/os/time.hpp>

int main()
{
    vx::io::println("hello world");

    const vx::time::datetime dt = vx::os::system_time().to_datetime();

    std::cout
        << "today is "
        << vx::time::weekday_to_string(dt.weekday) << ", "
        << vx::time::month_to_string(dt.month) << ' ' << dt.day << ", " << dt.year << '\n';
}
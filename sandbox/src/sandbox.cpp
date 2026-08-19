#include <cassert>
#include <cstring>
#include <forward_list>

#include "vertex/std/io.hpp"
#include "vertex/std/singly_linked_list.hpp"

using namespace vx;

using X = std::list<int>;
using X = std::forward_list<int>;

int main()
{
    single_linked_list<int> list(5, 5);

    {
        auto it = list.begin();
        size_t i = 2;

        while (i--)
        {
            ++it;
        }

        list.insert_after(it, { 1, 2, 3, 4 });
    }

    for (auto it = list.begin(); it != list.end(); ++it)
    {
        print(*it);
    }

    return 0;
}

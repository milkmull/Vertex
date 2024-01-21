#include <iostream>
#include <map>

#include "vertex/math/math.h"
#include "vertex/math/random/rng.h"

int main()
{
    using namespace vx;

    math::rng rng(0);

    std::vector<int> choices;
    std::vector<int> weights;
    std::map<int, int> tally;

    for (int i = 0; i < 10; i++)
    {
        choices.push_back(i);
        weights.push_back(i);

        tally[i] = 0;
    }

    math::weights w = weights;

    std::vector<int> choices_out(1000);
    rng.weighted_choices(choices.begin(), choices.end(), choices_out.begin(), choices_out.end(), w);

    for (int x : choices_out)
    {
        tally[x]++;
    }

    for (auto pair : tally)
    {
        std::cout << pair.first << ' ' << pair.second << std::endl;
    }

    return 0;
}
#pragma once

#include "vertex/util/random/pcg.hpp"

namespace vx {

class rng
{
public:



private:

    rand::pcg32<> m_rng;
};

} // namespace vx
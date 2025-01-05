#pragma once

namespace vx {
namespace iter {

template <typename Value, typename Owner>
class iterator_proxy
{
public:

    const Value& operator*() noexcept
    {
        return m_value;
    }

private:

    friend Owner;
    explicit iterator_proxy(const Value& v) : m_value(v) {}
    Value m_value;
};

} // namespace iter
} // namespace vx
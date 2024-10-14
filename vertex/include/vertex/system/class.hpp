#pragma once

#define VX_NON_COPYABLE(class_name)                    \
    class_name(const class_name&) = delete;            \
    class_name& operator=(const class_name&) = delete;

#define VX_MOVABLE_ONLY(class_name)                         \
    class_name(const class_name&) = delete;                 \
    class_name& operator=(const class_name&) = delete;      \
    class_name(class_name&&) noexcept = default;            \
    class_name& operator=(class_name&&) noexcept = default;
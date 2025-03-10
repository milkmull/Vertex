#include <iostream>
#include <string>

#if defined(_WIN32)
#   define SHARED_LIB_EXPORT __declspec(dllexport)
#else
#   define SHARED_LIB_EXPORT __attribute__((visibility("default")))
#endif

extern "C" {

    // Simple function returning a string
    SHARED_LIB_EXPORT const char* get_message()
    {
        return "Hello from the shared library!";
    }

    // Simple function performing addition
    SHARED_LIB_EXPORT int add(int a, int b)
    {
        return a + b;
    }

    // Function printing a message to standard output
    SHARED_LIB_EXPORT void print_message(const char* msg)
    {
        std::cout << "Message from library: " << msg << std::endl;
    }

} // extern "C"

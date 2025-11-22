
function(vx_parse_version_from_header FILE_PATH MAJOR_VAR MINOR_VAR PATCH_VAR)

    # Read the file content
    file(READ "${FILE_PATH}" VERSION_HEADER_CONTENT)

    # Extract the major, minor, and patch versions using regex
    string(REGEX MATCH "#define[ \t]+VX_VERSION_MAJOR[ \t]+([0-9]+)" _ ${VERSION_HEADER_CONTENT})
    set(${MAJOR_VAR} ${CMAKE_MATCH_1} PARENT_SCOPE)

    string(REGEX MATCH "#define[ \t]+VX_VERSION_MINOR[ \t]+([0-9]+)" _ ${VERSION_HEADER_CONTENT})
    set(${MINOR_VAR} ${CMAKE_MATCH_1} PARENT_SCOPE)

    string(REGEX MATCH "#define[ \t]+VX_VERSION_PATCH[ \t]+([0-9]+)" _ ${VERSION_HEADER_CONTENT})
    set(${PATCH_VAR} ${CMAKE_MATCH_1} PARENT_SCOPE)
    
endfunction()

#--------------------------------------------------------------------

function(print_option name description)

    if(DEFINED ${name})
        message(STATUS "  ${description}: ${${name}}")
    else()
        message(STATUS "  ${description}: <undefined>")
    endif()
    
endfunction()

#--------------------------------------------------------------------

# Helper function to hide public symbols by default in shared libraries.
# This reduces the size of the exported symbol table, improves load times,
# and enhances security by hiding internal implementation details.
# Only explicitly marked symbols (e.g., using __declspec(dllexport) on
# Windows or __attribute__((visibility("default"))) on Unix) will be visible.
# This setting only affects shared libraries and has no impact on static libraries.
function(vx_hide_public_symbols TARGET_NAME)

    set_target_properties(${TARGET_NAME} PROPERTIES
        CXX_VISIBILITY_PRESET hidden        # Hide all symbols by default
        VISIBILITY_INLINES_HIDDEN YES       # Hide inline functions unless explicitly marked
    )
    
endfunction()

#--------------------------------------------------------------------

# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html

function(vx_add_common_compiler_flags TARGET_NAME)

    # Enable C++17 support
    target_compile_features(${TARGET_NAME} PUBLIC cxx_std_17)

    option(VX_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" FALSE)

    if(VX_CMAKE_COMPILER_MSVC)
    
        target_compile_options(${TARGET_NAME} PRIVATE
            $<$<BOOL:${VX_WARNINGS_AS_ERRORS}>:/WX>
            /W4                         # Enable all Level 4 warnings (high level of warning verbosity)
            /w14242                     # Conversion from 'type1' to 'type2', possible loss of data
            /w14254                     # Ambiguous operators in 'if' and 'for' statements
            /w14263                     # '++' or '--' on a boolean variable
            /w14265                     # Class with virtual functions but without a virtual destructor
            /w14287                     # Returning address of a local variable
            /we4289                     # Nonstandard extension: 'loop control variable declared in the for-loop is used outside the loop scope'
            /w14296                     # Using an uninitialized variable
            /w14311                     # Conversion from a larger to a smaller integer type that could result in data loss
            /w14545                     # Using an 'else' statement that is not aligned with its 'if' statement
            /w14546                     # Ambiguity in overloaded operators
            /w14547                     # 'operator delete' called on an object without a virtual destructor
            /w14549                     # Creating a temporary object from a literal with no explicit copy constructor
            /w14555                     # Function signature mismatch (calling convention mismatch)
            /w14619                     # Using 'throw' in noexcept function
            /w14640                     # Conversion from smaller to larger enum type (possible data loss)
            /w14826                     # Using a comma operator in an 'if' or 'while' condition
            /w14905                     # 'override' specifier missing in an overridden virtual function
            /w14906                     # 'noexcept' specifier missing in a function expected to be noexcept
            /w14928                     # Member initializer list reorders members differently from class declaration
            /wd4100                     # The formal parameter is not referenced in the body of the function.
            /permissive-                # Disable permissive mode; makes MSVC more standards-compliant (strict mode)
        )
        
    endif()

    if(VX_CMAKE_COMPILER_GCC OR VX_CMAKE_COMPILER_CLANG)
        target_compile_options(${TARGET_NAME} PRIVATE
            $<$<BOOL:${VX_WARNINGS_AS_ERRORS}>:-Werror>
            -Wall                       # Enable commonly-used warning flags
            -Wextra                     # Enable additional warning flags not covered by -Wall
            -Wshadow                    # Warn when a variable declaration shadows one in an outer scope
            -Wnon-virtual-dtor          # Warn when a class with virtual functions does not have a virtual destructor
            -Wcast-align                # Warn on potential data loss when casting between types of different alignment
            -Wunused                    # Warn on unused variables, parameters, or functions
            -Woverloaded-virtual        # Warn when a function overload hides a virtual function from a base class
            -Wconversion                # Warn when a conversion could result in data loss
            -Wsign-conversion           # Warn on implicit conversions that change the sign of an integer
            -Wdouble-promotion          # Warn when a float is implicitly promoted to a double
            -Wformat=2                  # Enable stricter format string checking
            -Wimplicit-fallthrough      # Warn on missing break statements in switch cases
            -Wsuggest-override          # Suggest adding 'override' specifier to overridden virtual functions
            -Wnull-dereference          # Warn when a null pointer dereference is detected
            #-Wold-style-cast            # Warn when using C-style casts instead of C++ casts
            -Wpedantic                  # Enforce strict compliance with the language standard, emitting all the warnings demanded by the standard
        )
    endif()
    
    if(VX_CMAKE_COMPILER_GCC)
        # Don't enable -Wduplicated-branches for GCC < 8.1 since it will lead to false positives
        # https://github.com/gcc-mirror/gcc/commit/6bebae75035889a4844eb4d32a695bebf412bcd7
        target_compile_options(${TARGET_NAME} PRIVATE
            -Wmisleading-indentation    # Warn if indentation implies blocks where blocks do not exist
            -Wduplicated-cond           # Warn if if / else chain has duplicated conditions
            -Wlogical-op                # Warn about logical operations being used where bitwise were probably wanted
            -Wno-trigraphs              # Disable warnings for trigraphs
        )
    endif()
    
    # Disable certain deprecation warnings
    if(MSVC)
        target_compile_definitions(${TARGET_NAME} PRIVATE -D_CRT_SECURE_NO_DEPRECATE)
        target_compile_definitions(${TARGET_NAME} PRIVATE -D_CRT_NONSTDC_NO_DEPRECATE)
        target_compile_definitions(${TARGET_NAME} PRIVATE -D_CRT_SECURE_NO_WARNINGS)
    endif()
  
endfunction()

#--------------------------------------------------------------------

function(vx_create_source_groups TARGET_NAME ROOT_DIR)

    # Get the source files from the target
    get_target_property(SRC_FILES ${TARGET_NAME} SOURCES)
    if(NOT SRC_FILES)
        message(WARNING "Target ${TARGET_NAME} has no sources.")
        return()
    endif()

    foreach(SOURCE_FILE IN LISTS SRC_FILES)
        # Get the relative path of the source file from the root directory
        file(RELATIVE_PATH RELATIVE_FILE "${ROOT_DIR}" "${SOURCE_FILE}")
        
        # Get the directory of the relative file
        get_filename_component(RELATIVE_DIR "${RELATIVE_FILE}" DIRECTORY)
        
        # Add the relative path as a source group
        if(RELATIVE_DIR)
            string(REPLACE "/" "\\" GROUP_NAME "${RELATIVE_DIR}")
            source_group("${GROUP_NAME}" FILES "${SOURCE_FILE}")
        endif()
        
    endforeach()
    
endfunction()

#--------------------------------------------------------------------

# Enable AddressSanitizer if enabled
function(vx_enable_asan TARGET_NAME)

    message(STATUS "AddressSanitizer enabled for target: ${TARGET_NAME}")
    
    if(MSVC)
        #target_compile_options(${TARGET_NAME} PUBLIC "/fsanitize=address")
        #target_link_options(${TARGET_NAME} PUBLIC "/INCREMENTAL:NO")
    elseif(VX_CMAKE_COMPILER_GCC OR VX_CMAKE_COMPILER_CLANG)
        target_compile_options(${TARGET_NAME} PRIVATE "-fsanitize=address" "-fno-omit-frame-pointer")
        target_link_libraries(${TARGET_NAME} PRIVATE "-fsanitize=address")
    else()
        message(WARNING "AddressSanitizer is not supported for this compiler.")
    endif()
    
endfunction()

#--------------------------------------------------------------------

# Function to check if a function exists on the platform
# and define a preprocessor directive if it does
function(vx_check_function_exists TARGET_NAME FUNCTION_NAME MACRO_NAME)

    include(CheckFunctionExists)
    # Check if the function exists
    check_function_exists(${FUNCTION_NAME} ${MACRO_NAME})

    # If the function exists, add the definition to the target
    if(${MACRO_NAME})
        target_compile_definitions(${TARGET_NAME} PRIVATE ${MACRO_NAME})
    endif()
    
endfunction()

#--------------------------------------------------------------------

# Function to check if a symbol exists on the platform
# and define a preprocessor directive if it does
function(vx_check_symbol_exists TARGET_NAME SYMBOL_NAME HEADER MACRO_NAME)

    include(CheckSymbolExists)
    # Check if the symbol exists in the given header
    check_symbol_exists(${SYMBOL_NAME} ${HEADER} ${MACRO_NAME})

    # If the symbol exists, add the definition to the target
    if(${MACRO_NAME})
        target_compile_definitions(${TARGET_NAME} PRIVATE ${MACRO_NAME})
    endif()

endfunction()

#--------------------------------------------------------------------

# Function to check if an include file exists on the platform
# and define a preprocessor directive if it does
function(vx_check_include_exists TARGET_NAME INCLUDE_NAME MACRO_NAME)

    include(CheckIncludeFile)
    # Check if the header exists
    check_include_file(${INCLUDE_NAME} ${MACRO_NAME})

    # If the header exists, add the definition to the target
    if(${MACRO_NAME})
        target_compile_definitions(${TARGET_NAME} PRIVATE ${MACRO_NAME})
    endif()

endfunction()

#--------------------------------------------------------------------
# Function to check if a struct has a specific member
# and define a preprocessor directive if it does
function(vx_check_struct_member TARGET_NAME STRUCT_NAME MEMBER_NAME HEADER MACRO_NAME)

    include(CheckStructHasMember)
    # Check if the struct has the specified member
    check_struct_has_member("${STRUCT_NAME}" ${MEMBER_NAME} ${HEADER} ${MACRO_NAME})

    # If the member exists, add the definition to the target
    if(${MACRO_NAME})
        target_compile_definitions(${TARGET_NAME} PRIVATE ${MACRO_NAME})
    endif()

endfunction()
#--------------------------------------------------------------------
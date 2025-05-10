#pragma once

#include <string>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace os {
namespace locale {

/**
 * @brief Retrieves the country code.
 *
 * This function fetches the country code (e.g., "US" for the United States, "DE" for Germany)
 * and stores it in the provided `std::string` reference.
 *
 * @param[out] country_code A reference to a `std::string` where the country code will be stored.
 *
 * @return `true` if the country code was successfully retrieved, `false` otherwise.
 */
VX_API bool get_country_code(std::string& country_code);

/**
 * @brief Retrieves the language code.
 *
 * This function fetches the language code (e.g., "en" for English, "de" for German)
 * and stores it in the provided `std::string` reference.
 *
 * @param[out] language A reference to a `std::string` where the language code will be stored.
 *
 * @return `true` if the language code was successfully retrieved, `false` otherwise.
 */
VX_API bool get_language(std::string& language);

} // namespace locale
} // namespace os
} // namespace vx
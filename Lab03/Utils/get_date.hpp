//
// Created by Piotr Moszkowicz on 21/10/2019.
//

#pragma once

#include <ctime>
#include <string>

namespace Utils {
    /**
     * @fn get_http_data
     * @brief Gets current date in the HTTP correct format
     * @return [std::string]            -   String with the date
     */
    std::string get_http_date();

    /**
     * @fn get_day_shortcut
     * @param [int] day_number          -   Number of the day in the week
     * @return [const char*]            -   Shortcut of the day in the week
     */
    const char *get_day_shortcut(int day_number);

    /**
     * @fn get_month_shortcut
     * @param [int] month_number        -   Number of the month in the year
     * @return [const char*]            -   Shortcut of the month in the year
     */
    const char *get_month_shortcut(int month_number);
}

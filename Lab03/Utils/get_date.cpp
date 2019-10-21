//
// Created by Piotr Moszkowicz on 21/10/2019.
//

#include "get_date.hpp"

std::string Utils::get_http_date() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::string date = "Date: ";

    date += Utils::get_day_shortcut(now->tm_wday);
    date += ", ";
    date += std::to_string(now->tm_mday) + " ";
    date += Utils::get_month_shortcut(now->tm_mon);
    date += " " + std::to_string(now->tm_year) + " ";
    date += std::to_string(now->tm_hour) + " ";
    date += std::to_string(now->tm_min) + " ";
    date += std::to_string(now->tm_sec) + " ";
    date += "GMT+2";
    date += "\n";

    return date;
}

const char* Utils::get_day_shortcut(int day_number) {
    switch (day_number) {
        case 0:
            return "Sun";
        case 1:
            return "Mon";
        case 2:
            return "Tue";
        case 3:
            return "Wed";
        case 4:
            return "Thu";
        case 5:
            return "Fri";
        case 6:
            return "Sat";
        default:
            return "Sun";
    }
}

const char* Utils::get_month_shortcut(int month_number) {
    switch (month_number) {
        case 0:
            return "Jan";
        case 1:
            return "Feb";
        case 2:
            return "Mar";
        case 3:
            return "Apr";
        case 4:
            return "May";
        case 5:
            return "Jun";
        case 6:
            return "Jul";
        case 7:
            return "Aug";
        case 8:
            return "Sep";
        case 9:
            return "Oct";
        case 10:
            return "Nov";
        case 11:
            return "Dec";
        default:
            return "Jan";
    }
}
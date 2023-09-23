#include "../inc/Utils.hpp"

std::string getTime() {
    // Get the current time point in the system clock
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    // Convert the time point to a time_t object
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);

    // Convert the time_t object to a tm struct for easier time manipulation 
    std::tm* time_info = std::localtime(&current_time);

    // Format the timestamp as a string, using "string format time"
    char time_str[100];
    std::strftime(time_str, sizeof(time_str), "[%H:%M:%S] ", time_info);

    return time_str;
}
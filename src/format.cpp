#include <string>

#include "format.h"

using std::string;

// Helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    int hr;
    int min;
    int sec;
    hr = seconds/3600;
    min = (seconds%3600)/60;
    sec = seconds%60;

    string output;
    output = std::to_string(hr) + ":" + std::to_string(min) + ":" + std::to_string(sec);
    return output;
}
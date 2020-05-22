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
    hr = seconds / 3600;
    min = (seconds % 3600) / 60;
    sec = seconds % 60;

    string hr_str = hr > 9 ? std::to_string(hr) : "0" + std::to_string(hr);
    string min_str = min > 9 ? ":" + std::to_string(min) : ":0" + std::to_string(min);
    string sec_str = sec > 9 ? ":" + std::to_string(sec) : ":0" + std::to_string(sec);

    return hr_str + min_str + sec_str;
}
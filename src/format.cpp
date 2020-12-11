#include <string>
#include <iomanip>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {     
    std::ostringstream stream;
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int seconds_ = seconds - (hours * 3600 + minutes * 60);
    stream << std::setw(2) << std::setfill('0') << hours << ":";
    stream << std::setw(2) << std::setfill('0') << minutes << ":";
    stream << std::setw(2) << std::setfill('0') << seconds_;    
    return stream.str(); 
}
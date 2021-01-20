#include <string>
#include <sstream>
#include <iomanip>
#include "format.h"

using std::string;

// change int to 2-digits string
string Format::SetFormat(int i){
    std::stringstream line;
    if(i>=0 && i<100){
        line << std::setw(2) << std::setfill('0')<<i;
    }
  
    return line.str();
}

// change float to 2-digits string
string Format::SetFormat(float i){
    std::stringstream line;
    line << std::fixed << std::setprecision(2)<<i; 
    string line_s = line.str();
    return line.str();
}
// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    int minutes{0},hours{0},display_minutes{0};
    hours = seconds /3600;
    minutes = seconds / 60 - hours * 60;   
    display_minutes = seconds - 3600*hours -60*minutes;    
    string out = SetFormat(hours) +":"+ SetFormat(minutes) + ":" + SetFormat(display_minutes);
    return out; 
}
#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string SetFormat(float i);
std::string SetFormat(int i);
std::string ElapsedTime(long times);  // TODO: See src/format.cpp
};                                    // namespace Format

#endif
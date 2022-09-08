#include <memory>
#include <string>
#include <stdexcept>

template<typename ... Args>
std::string format(const std::string& formatString, Args ... args);
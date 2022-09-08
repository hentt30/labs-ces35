#include "utils.h"

template<typename ... Args>
std::string format(const std::string& formatString, Args ... args)
{
    int size_s = std::snprintf(nullptr,0,format.c_str(),args ...) + 1; // get the size of the formatting string and dont store it nowhere
    if(size_s <= 0){ throw std::runtime_error("Error during format");}// if the formating was wrong, throw an exception
    size_t size = static_cast<size_t>(size_s); // resolve the cast at compile time
    std::unique_ptr<char[]> buf(new char[size]); // create an pointer to an buffer that will store the format string
    std::snprintf( buf.get(), size, format.c_str(), args ...); // now execute the snprintf to store the formatted string in the buffer
    return std::string(buf.get(),buf.get()+size-1); // construct a new string with the pointer generated above
}
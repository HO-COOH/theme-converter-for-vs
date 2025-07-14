#pragma once
#include <stdexcept>

namespace ThemeConverterCppLib
{
    class InvalidInputFileError : public std::invalid_argument
    {
    public:
        InvalidInputFileError();
    };
}
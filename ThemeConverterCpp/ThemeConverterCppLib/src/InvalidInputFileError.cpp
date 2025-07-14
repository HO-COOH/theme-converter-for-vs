#include "InvalidInputFileError.h"

namespace ThemeConverterCppLib
{
    InvalidInputFileError::InvalidInputFileError()
        : invalid_argument{ "Invalid input JSON file"}
    {
    }
}
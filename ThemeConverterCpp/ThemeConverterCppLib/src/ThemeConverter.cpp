#include "ThemeConverter.h"
#include "InvalidInputFileError.h"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <iterator>
#include <fstream>

namespace ThemeConverterCppLib 
{
    void Convert(std::string_view input, std::string_view output)
    {
        std::filesystem::path inputPath{input};
        if (!std::filesystem::is_regular_file(inputPath) || inputPath.extension() != ".json")
            throw InvalidInputFileError{};
        
        std::ifstream inputFile{inputPath};
        auto themeJson = nlohmann::json::parse(
            std::string{std::istreambuf_iterator<char>{inputFile}, std::istreambuf_iterator<char>{}}
        );
    }
    void ShowHelp()
    {

    }
} // namespace ThemeConverterCppLib 
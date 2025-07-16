#include "ThemeConverter.h"
#include "InvalidInputFileError.h"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <iterator>
#include <fstream>
#include "VSCode/Theme.h"
#include <boost/uuid.hpp>
#include <format>
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include "VSCode/TokenMapping.h"

template<>
struct std::formatter<boost::uuids::uuid> : std::formatter<std::string>
{
    auto format(boost::uuids::uuid const& value, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "{}", boost::uuids::to_string(value));
    }
};

namespace ThemeConverterCppLib 
{
    constexpr static boost::uuids::uuid DarkThemeId{{
        0x1d, 0xed, 0x01, 0x38,
        0x47, 0xce,
        0x43, 0x5e,
        0x84, 0xef,
        0x9e, 0xc1, 0xf4, 0x39, 0xb7, 0x49
    }};

    constexpr static boost::uuids::uuid LightThemeId{{
        0xde, 0x3d, 0xbb, 0xcd,
        0xf6, 0x42,
        0x43, 0x3c,
        0x83, 0x53,
        0x8f, 0x1d, 0xf4, 0x37, 0x0a, 0xba
    }};

    static std::unordered_map<std::string, std::unordered_map<std::string, VSCode::Theme::TokenColors_::Settings_>> groupColorsByCategory(
        VSCode::Theme const& themeFile
    ) 
    {
        auto file = nlohmann::json::parse(VSCode::TokenMapping);
        return {};
    }

    void Convert(std::string_view input, std::string_view output)
    {
        std::filesystem::path inputPath{input};
        if (!std::filesystem::is_regular_file(inputPath) || inputPath.extension() != ".json")
            throw InvalidInputFileError{};
        
        std::ifstream inputFile{inputPath};
        VSCode::Theme theme{nlohmann::json::parse(
            std::string{std::istreambuf_iterator<char>{inputFile}, std::istreambuf_iterator<char>{}}
        )};

        std::ofstream outputFile{output.data()};
        outputFile << "<Themes>\n";

        boost::uuids::uuid themeGuid{boost::uuids::random_generator{}()};
        outputFile << std::format(
            R"(    <Theme Name="{}" GUID="{}" FallbackId="{}")
            )",
            theme.Name(),
            themeGuid,
            theme.Type() == VSCode::Theme::Type_::Dark? DarkThemeId : LightThemeId
        );
        for (auto &&[key, value] : groupColorsByCategory(theme))
        {
            outputFile << R"(        <Category Name="{}" GUID="{}">
            )";
            for (auto && color : value)
            {
                if(color.second)
            }
            
        }
        
    }
    void ShowHelp()
    {

    }
} // namespace ThemeConverterCppLib 
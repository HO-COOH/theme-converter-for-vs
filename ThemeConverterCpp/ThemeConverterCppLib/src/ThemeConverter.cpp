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
#include "VS/VSTokens.h"
#include <uuid_parse.hpp>

using namespace guid_parse::literals;

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

    // constexpr static boost::uuids::uuid DarkThemeId{{
    //     0x1d, 0xed, 0x01, 0x38,
    //     0x47, 0xce,
    //     0x43, 0x5e,
    //     0x84, 0xef,
    //     0x9e, 0xc1, 0xf4, 0x39, 0xb7, 0x49
    // }};

    constexpr static boost::uuids::uuid DarkThemeId = "1ded0138-47ce-435e-84ef-9ec1f439b749"_uuid;
    constexpr static boost::uuids::uuid LightThemeId = "de3dbbcd-f642-433c-8353-8f1df4370aba"_uuid;

    // constexpr static boost::uuids::uuid LightThemeId{{
    //     0xde, 0x3d, 0xbb, 0xcd,
    //     0xf6, 0x42,
    //     0x43, 0x3c,
    //     0x83, 0x53,
    //     0x8f, 0x1d, 0xf4, 0x37, 0x0a, 0xba
    // }};

    struct ColorKey
    {
        std::string categoryName;
        std::string keyName;
        std::string aspect;
        std::optional<float> foregroundOpacity;
        std::optional<std::string> vscBackground;
        bool isBackground;

        ColorKey(
            std::string categoryName,
            std::string keyName,
            std::string backgroundOrForeground,
            std::optional<std::string> foregroundOpacity = {},
            std::optional<std::string> vscBackground = {}
        ) : categoryName{std::move(categoryName)},
            keyName{std::move(keyName)},
            aspect{std::move(backgroundOrForeground)},
            vscBackground{std::move(vscBackground)}
        {
            isBackground = (aspect == "background" || aspect == "Background");
            if(foregroundOpacity)
                this->foregroundOpacity = std::stof(*foregroundOpacity);
        }
    };

    static std::vector<std::string> mappedVSTokens;

    static void checkForMissingVSTokens()
    {
        if(mappedVSTokens.empty())
            return;

        
    }

    static std::unordered_map<std::string, std::vector<ColorKey>> createScopeMapping()
    {
        auto file = nlohmann::json::parse(VSCode::TokenMapping, nullptr, true, true, true);
        std::unordered_map<std::string, std::vector<ColorKey>> scopeMappings;
        for (auto&& color : file["tokenColors"])
        {
            std::vector<ColorKey> values;
            for(auto&& vsToken : color["VSC Token"])
            {
                std::vector<std::string> colorKey;
                boost::algorithm::split(colorKey, vsToken.get<std::string>(), [](char c){return c == '&';});
                switch (colorKey.size())
                {
                    case 2:
                        values.emplace_back(std::move(colorKey[0]), std::move(colorKey[1]), "Foreground");
                        break;
                    case 3:
                        values.emplace_back(std::move(colorKey[0]), std::move(colorKey[1]), std::move(colorKey[2]));
                        break;
                    case 4:
                        values.emplace_back(
                            std::move(colorKey[0]), 
                            std::move(colorKey[1]), 
                            "Foreground", 
                            std::move(colorKey[2]), 
                            std::move(colorKey[3])
                        );
                        break;
                    case 5:
                        values.emplace_back(
                            std::move(colorKey[0]),
                            std::move(colorKey[1]),
                            std::move(colorKey[2]),
                            std::move(colorKey[3]),
                            std::move(colorKey[4])
                        );
                        break;
                    default:
                        throw std::runtime_error{"Invalid mapping format"};
                }

                auto& newColorKey = values.back();
                mappedVSTokens.emplace_back(std::format("{}&{}&{}", newColorKey.categoryName, newColorKey.keyName, newColorKey.aspect));
            }
            scopeMappings.emplace(color["VSC Token"].get<std::string>(),std::move(values));
        }

        checkForMissingVSTokens();
        return scopeMappings;
    }

    static std::unordered_map<std::string, std::unordered_map<std::string, VSCode::Theme::TokenColors_::Settings_>> groupColorsByCategory(
        VSCode::Theme const& themeFile
    ) 
    {
        std::unordered_map<std::string, std::unordered_map<std::string, VSCode::Theme::TokenColors_::Settings_>> colorCategories;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> assignBy;
        std::unordered_map<std::string, bool> keyUsed;
        auto scopeMappings = createScopeMapping();
        for(auto&& [key, _] : scopeMappings)
            keyUsed[key] = false;

        if(auto tokenColors = themeFile.TokenColors())
        {
            for(auto&& ruleContract : tokenColors)
            {

            }
        }
        return colorCategories;
    }

    static std::string reviseColor(std::string const& color)
    {
        std::string_view revisedColor = color[0] == '#'? std::string_view{color}.substr(1) : std::string_view{color};
        switch (revisedColor.size())
        {
            case 3:
                return std::format("FF{0}{0}{1}{1}{2}{2}", 
                    revisedColor.substr(0, 1),
                    revisedColor.substr(1, 1),
                    revisedColor.substr(2, 1)
                );
            case 4:
                return std::format("{0}{0}{1}{1}{2}{2}{3}{3}",
                    revisedColor.substr(0, 1),
                    revisedColor.substr(1, 1),
                    revisedColor.substr(2, 1),
                    revisedColor.substr(3, 1)
                );
            case 6:
                return std::format("FF{}", revisedColor);
            case 8:
                return std::format("{}{}", revisedColor.substr(6), revisedColor.substr(0, 6));
            default:
                return std::string{revisedColor};
        }
    }

    static void writeColor(
        std::ostream& writer, 
        std::string const& colorKeyName, 
        std::optional<std::string> const& foregroundColor,
        std::optional<std::string> const& backgroundColor)
    {
        writer << std::format(R"(            <Color Name="{}">)", colorKeyName);
        if (foregroundColor)
            writer << std::format(R"(                <Background Type="CT_RAW" Source="{}"/>)", reviseColor(*backgroundColor));
        if (backgroundColor)
            writer << std::format(R"(                <Foreground Type="CT_RAW" Source="{}"/>)", reviseColor(*foregroundColor));
        writer << "            </Color>";
    }

    void Convert(std::string_view input, std::string_view output)
    {
        std::filesystem::path inputPath{input};
        if (!std::filesystem::is_regular_file(inputPath))
            throw InvalidInputFileError{};
        
       
        VSCode::Theme theme{std::ifstream{inputPath}};

        std::ofstream outputFile{output.data()};
        outputFile << "<Themes>\n";

        boost::uuids::uuid themeGuid{boost::uuids::random_generator{}()};
        outputFile << std::format(
            R"(    <Theme Name="{}" GUID="{}" FallbackId="{}")
            )",
            theme.Name().value_or(inputPath.filename().string()),
            themeGuid,
            theme.Type() == VSCode::Theme::Type_::Dark? DarkThemeId : LightThemeId
        );
        for (auto &&[key, value] : groupColorsByCategory(theme))
        {
            outputFile << R"(        <Category Name="{}" GUID="{}">
            )";
            for (auto && color : value)
            {
                auto foreground = color.second.Foreground();
                auto background = color.second.Background();
                if(foreground || background)
                    writeColor(outputFile, key, foreground, background);
            }
            outputFile << R"(        </Category>)";
            
        }
        
        outputFile << "    </Theme>\n";
        outputFile << "</Theme>";
    }
    void ShowHelp()
    {

    }
} // namespace ThemeConverterCppLib 
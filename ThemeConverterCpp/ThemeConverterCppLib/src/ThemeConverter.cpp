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
#include "VSCode/TokenFallback.h"
#include "VS/VSTokens.h"
#include "VS/OverlayMapping.h"
#include <uuid_parse.hpp>
#include "ColorUtils.h"

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
            for(auto&& vsToken : color["VS Token"])
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

    static std::unordered_map<std::string, std::string> createVSCTokenFallback()
    {
        std::unordered_map<std::string, std::string> ret;
        auto file = nlohmann::json::parse(VSCode::TokenFallback);
        for(auto&& item : file.items())
        {
            ret.insert({item.key(), item.value()});
        }
        return ret;
    }

    static void assignEditorColors(
        std::vector<ColorKey> colorKeys,
        std::string const& scope,
        VSCode::Theme::TokenColors_::TokenColor_& ruleContract,
        std::unordered_map<std::string, std::unordered_map<std::string, VSCode::Theme::TokenColors_::TokenColor_::Settings_>>& colorCategories,
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& assignBy
    )
    {
        for(auto const& colorKey : colorKeys)
        {
            auto ruleList = colorCategories.try_emplace(colorKey.categoryName).first; //iterator to <string, map>
            auto assignList = assignBy.try_emplace(colorKey.categoryName).first;
            if(auto iter = ruleList->second.find(colorKey.keyName); iter != ruleList->second.end())
            {
                if(scope.starts_with(assignList->second[colorKey.keyName]) && ruleContract.Settings().Foreground())
                {
                    std::construct_at(std::addressof(iter->second), ruleContract.Settings());
                    assignList->second[colorKey.keyName] = scope;
                }
            }
            else
            {
                ruleList->second.insert({colorKey.keyName, ruleContract.Settings()});
                assignList->second.insert({colorKey.keyName, scope});
            }
        }
    }

    static auto tryGetValue(auto&& key, auto&& map)
    {
        auto iter = map.find(key);
        return iter != map.end()? &iter->second : nullptr;
    }

    template<typename T>
    static auto tryGetValueJson(auto const& key, nlohmann::json const& json)
    {
        auto iter = json.find(key);
        return iter != json.end()? std::optional{iter->get<T>()} : std::nullopt;
    }

    static auto vscTokenFallback = createVSCTokenFallback();

    std::optional<std::string> tryGetColorValue(VSCode::Theme::Colors_ const& themeColors, std::string_view token)
    {
        auto colorValue = tryGetValueJson<std::string>(token, themeColors.Get());
        
        std::string key{token};
        while(colorValue)
        {
            if(auto fallbackToken = tryGetValue(key, vscTokenFallback))
            {
                key = std::move(*fallbackToken);
                colorValue = tryGetValueJson<std::string>(key, themeColors.Get());
            }
            else
                break;
        }

        return colorValue;
    }

    static void assignShellColor(
        VSCode::Theme const& theme,
        std::string colorValue,
        std::vector<ColorKey> colorKeys,
        std::unordered_map<std::string, std::unordered_map<std::string, VSCode::Theme::TokenColors_::TokenColor_::Settings_>>& colorCategories
    )
    {
        auto const& themeColors = theme.Colors();
        for(auto const& colorKey : colorKeys)
        {
            if(colorKey.foregroundOpacity && colorKey.vscBackground)
            {
                if(auto backgroundColor = tryGetColorValue(themeColors, *colorKey.vscBackground))
                    colorValue = GetCompoundColor(colorValue, *backgroundColor, 1, *colorKey.foregroundOpacity);
            }

            auto rulesList = tryGetValue(colorKey.categoryName, colorCategories);
            if(!rulesList)
                rulesList = &(colorCategories.insert({colorKey.categoryName, std::unordered_map<std::string, VSCode::Theme::TokenColors_::TokenColor_::Settings_>{}}).first->second);
            auto colorSettings = tryGetValue(colorKey.keyName, *rulesList);
            if(!colorSettings)
                colorSettings = &(rulesList->insert({colorKey.keyName, VSCode::Theme::TokenColors_::TokenColor_::Settings_{}}).first->second);
            if(colorKey.isBackground)
                colorSettings->Background(colorValue);
            else
                colorSettings->Foreground(colorValue);
        }
    }

    static std::unordered_map<std::string, std::unordered_map<std::string, VSCode::Theme::TokenColors_::TokenColor_::Settings_>> groupColorsByCategory(
        VSCode::Theme const& themeFile
    ) 
    {
        std::unordered_map<std::string, std::unordered_map<std::string, VSCode::Theme::TokenColors_::TokenColor_::Settings_>> colorCategories;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> assignBy;

        auto scopeMappings = createScopeMapping();

        std::unordered_map<std::string_view, bool> keyUsed;
        keyUsed.reserve(scopeMappings.size());
        keyUsed.insert_range(scopeMappings | std::views::transform([](auto&& mapping)
        {
            return std::pair{std::string_view{mapping.first}, false};
        }));

        auto tokenColors = themeFile.TokenColors();
        if(tokenColors)
        {
            for(auto&& ruleContract : tokenColors)
            {
                for(auto&& scopeName : ruleContract.Scope())
                {
                    for(auto iter = boost::make_split_iterator(scopeName, boost::token_finder(boost::is_any_of(","))); !iter.eof(); ++iter)
                    {
                        auto&& range = *iter;
                        std::string scope{range.begin(), range.end()};
                        boost::trim(scope);
                        
                        for (auto&& [key, colorKeys] : scopeMappings)
                        {
                            if(!scope.empty() && key.starts_with(scope))
                            {
                                keyUsed[key] = true;
                                assignEditorColors(
                                    colorKeys,
                                    scope,
                                    ruleContract,
                                    colorCategories,
                                    assignBy
                                );
                            }
                        }
                        
                    }
                }
            }
        }
        
        auto colors = themeFile.Colors();
        for(auto&& [key, used] : keyUsed)
        {
            if(used)
                continue;

            std::string keyString{key};
            if(auto iter = vscTokenFallback.find(keyString); iter != vscTokenFallback.end())
            {
                auto const& fallbackToken = iter->second;
                if(fallbackToken == "foreground")
                {
                    if(auto iter = colors.Get().find("foreground"); iter != colors.Get().end())
                    {
                        if(auto colorKeys = tryGetValue(keyString, scopeMappings))
                        {
                            assignShellColor(
                                themeFile,
                                iter->get<std::string>(),
                                *colorKeys,
                                colorCategories
                            );
                        }
                    }
                }
                
                if(tokenColors)
                {
                    for(auto&& ruleContract : themeFile.TokenColors())
                    {
                        for(auto&& scopeName : ruleContract.Scope())
                        {
                            for(auto iter = boost::make_split_iterator(scopeName, boost::token_finder(boost::is_any_of(","))); !iter.eof(); ++iter)
                            {
                                auto&& range = *iter;
                                std::string scope{range.begin(), range.end()};
                                boost::trim(scope);
                                if(!scope.empty() && fallbackToken.starts_with(scope))
                                {
                                    if(auto colorKeys = tryGetValue(keyString, scopeMappings))
                                    {
                                        assignEditorColors(
                                            *colorKeys,
                                            scope,
                                            ruleContract,
                                            colorCategories,
                                            assignBy
                                        );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        //Add the shell colors
        auto overlayMapping = nlohmann::json::parse(VS::OverlayMapping);
        for(auto&& color : colors.Get().items())
        {
            auto colorKey = color.key();
            if(auto colorKeyList = tryGetValue(boost::trim_copy(colorKey), scopeMappings))
            {
                auto colorValue = tryGetColorValue(colors, colorKey);
                if(!colorValue)
                    continue;

                if(auto item = overlayMapping.find(colorKey); item != overlayMapping.end())
                {
                    if(auto backgroundColor = tryGetColorValue(colors, item->at("Item2")))
                    {
                        colorValue = GetCompoundColor(*colorValue, *backgroundColor, item->at("Item1").get<float>());
                    }
                }

                assignShellColor(
                    themeFile,
                    *colorValue,
                    *colorKeyList,
                    colorCategories
                );
            }
        }
        return colorCategories;
    }

    static void writeColor(
        std::ostream& writer, 
        std::string const& colorKeyName, 
        std::optional<std::string> const& foregroundColor,
        std::optional<std::string> const& backgroundColor)
    {
        writer << std::format(R"(            <Color Name="{}">)", colorKeyName);
        if (foregroundColor)
            writer << std::format(R"(                <Background Type="CT_RAW" Source="{}"/>)", ReviseColor(*backgroundColor));
        if (backgroundColor)
            writer << std::format(R"(                <Foreground Type="CT_RAW" Source="{}"/>)", ReviseColor(*foregroundColor));
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
            outputFile << "        </Category>\n";
            
        }
        
        outputFile << "    </Theme>\n";
        outputFile << "</Theme>";
    }
    void ShowHelp()
    {

    }
} // namespace ThemeConverterCppLib 
//https://github.com/wraith13/vscode-schemas/blob/master/en/latest/schemas/color-theme.json
#pragma once
#include <nlohmann/json.hpp>

namespace ThemeConverterCppLib
{
    namespace VSCode
    {
        class Theme
        {
            nlohmann::json m_json;
        public:

            class TokenColors
            {
                nlohmann::json& m_json;
            public:
                TokenColors(nlohmann::json& value);

                std::string Name();
            };

            class Colors
            {
                nlohmann::json& m_json;
            public:
                Colors(nlohmann::json& value);

                
            };

            Theme(nlohmann::json&& value);

            std::string Name();
            bool SemanticHighlighting();
            TokenColors TokenColors();
        };
    }
}
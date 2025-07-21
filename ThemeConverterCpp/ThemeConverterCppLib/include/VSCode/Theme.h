//https://github.com/wraith13/vscode-schemas/blob/master/en/latest/schemas/color-theme.json
#pragma once
#include <nlohmann/json.hpp>

namespace ThemeConverterCppLib::VSCode
{
    class Theme
    {
        nlohmann::json m_json;
    public:

        class TokenColors_
        {
            nlohmann::json const& m_json;
        public:
            
            class Settings_
            {
                nlohmann::json const& m_json;
            public:
                Settings_(nlohmann::json const& value);
                std::optional<std::string> Foreground() const;
                std::optional<std::string> Background() const;
            };
            TokenColors_(nlohmann::json const& value);

            operator bool() const;

            auto begin() const { return m_json.begin(); }

            auto end() const { return m_json.end(); }
        };

        class Colors_
        {
            nlohmann::json const& m_json;
        public:
            Colors_(nlohmann::json const& value);

            class Color
            {
                nlohmann::json const& m_json;
            public:
                Color(nlohmann::json const& value);
            };
            Color operator[](std::string_view name) const;
        };

        enum class Type_
        {
            Dark,
            Light
        };
        Theme(nlohmann::json&& value);
        Theme(std::ifstream&& file);

        Type_ Type() const;
        std::optional<std::string> Name() const;
        Colors_ Colors() const;
        bool SemanticHighlighting() const;
        TokenColors_ TokenColors() const;
    };
}

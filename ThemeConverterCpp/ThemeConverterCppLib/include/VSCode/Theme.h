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
            
            class TokenColor_
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

                class Scope_
                {
                    nlohmann::json const& m_json;
                public:
                    Scope_(nlohmann::json const& value) : m_json{value}{}
                    
                    class Iterator
                    {
                        nlohmann::json const* m_json;
                        int m_index{};
                        bool m_isArray;
                    public:
                        Iterator(nlohmann::json const* json) : m_json{json}
                        {
                            if(m_json)
                                m_isArray = m_json->is_array();
                        }

                        Iterator(nlohmann::json const* json, int size) : Iterator{json} 
                        {
                            m_index = size;
                        }

                        std::string operator*() const
                        {
                            if (m_isArray)
                                return (*m_json)[m_index].get<std::string>();
                            return m_json->get<std::string>();
                        }

                        auto& operator++() { ++m_index; return *this; }
                        auto operator<=>(Iterator const&) const = default;
                    };

                    auto begin() const { return Iterator{&m_json};}
                    auto end() const { return Iterator{&m_json, m_json.is_array()? static_cast<int>(m_json.size()) : 1}; } 
                };

                TokenColor_(nlohmann::json const& json);
                Settings_ Settings();
                Scope_ Scope();
            };
            TokenColors_(nlohmann::json const& value);

            operator bool() const;

            class Iterator
            {
                nlohmann::json::const_iterator m_iter;
            public:
                Iterator(nlohmann::json::const_iterator iter) : m_iter{iter}{}
                TokenColor_ operator*() const{return TokenColor_{*m_iter};}
                auto& operator++() { ++m_iter; return *this; }
                auto operator++(int) { auto tmp = *this; ++(*this); return tmp; }
                auto operator<=>(Iterator const&) const = default;
            };

            auto begin() const { return Iterator{m_json.begin()}; }
            auto end() const { return Iterator{m_json.end()}; }
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

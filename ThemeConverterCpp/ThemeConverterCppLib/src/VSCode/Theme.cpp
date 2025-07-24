#include "VSCode/Theme.h"
#include <fstream>
#include <iterator>

namespace ThemeConverterCppLib::VSCode
{
    Theme::Theme(std::ifstream&& inputFile) : Theme{
        nlohmann::json::parse(
            std::string{std::istreambuf_iterator<char>{inputFile}, std::istreambuf_iterator<char>{}}
        )
    }
    {
    }

    Theme::Theme(nlohmann::json&& value) : m_json(std::move(value))
    {
    }

    std::optional<std::string> Theme::Name() const
    {
        if (auto iter = m_json.find("name"); iter != m_json.end())
            return iter->get<std::string>();
        return {};
    }

    Theme::Colors_ Theme::Colors() const
    {
        return m_json["color"];
    }

    bool Theme::SemanticHighlighting() const
    {
        return m_json["semanticHighlighting"].get<bool>();
    }

    Theme::TokenColors_ Theme::TokenColors() const
    {
        return m_json["tokenColors"];
    }

    Theme::TokenColors_::TokenColors_(nlohmann::json const &value) : m_json{value}
    {
    }

   Theme::TokenColors_::operator bool() const
    {
        return !m_json.is_null() && !m_json.empty();
    }

    Theme::TokenColors_::TokenColor_::TokenColor_(nlohmann::json const& json) : m_json{json}
    {
    }

    Theme::TokenColors_::TokenColor_::Settings_ Theme::TokenColors_::TokenColor_::Settings()
    {
        return Settings_{m_json["settings"]};
    }
   
    Theme::TokenColors_::TokenColor_::Scope_ Theme::TokenColors_::TokenColor_::Scope()
    {
        return Scope_{m_json["scope"]};
    }

    Theme::TokenColors_::TokenColor_::Settings_::Settings_(nlohmann::json const& value) : m_json{value}
    {
    }

    std::optional<std::string> Theme::TokenColors_::TokenColor_::Settings_::Foreground() const
    {
        if (auto iter = m_json.find("foreground"); iter != m_json.end())
            return iter->get<std::string>();
        return {};
    }

    std::optional<std::string> Theme::TokenColors_::TokenColor_::Settings_::Background() const
    {
        if (auto iter = m_json.find("background"); iter != m_json.end())
            return iter->get<std::string>();
        return {};
    }

    Theme::Colors_::Colors_(nlohmann::json const& value) : m_json{value}
    {
    }

    Theme::Colors_::Color::Color(nlohmann::json const& value) : m_json{value}
    {
    }

    Theme::Type_ Theme::Type() const
    {
        return Type_::Dark;    
    }
}

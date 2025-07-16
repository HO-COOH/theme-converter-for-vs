#include "VSCode/Theme.h"

namespace ThemeConverterCppLib::VSCode
{
    Theme::Theme(nlohmann::json&& value) : m_json{std::move(value)}
    {
    }

    std::string Theme::Name() const
    {
        return m_json["name"];
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

    Theme::TokenColors_::Settings_::Settings_(nlohmann::json const& value) : m_json{value}
    {
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

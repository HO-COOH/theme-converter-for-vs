#include "ColorUtils.h"
#include <format>
#include <string_view>
#include <algorithm>

namespace ThemeConverterCppLib
{
    std::string ReviseColor(std::string const& color)
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

    static std::string getCompoundColorImpl(
        std::string const& overlayColor,
        std::string const& baseColor,
        float VSOpacity,
        float VSCOpacity
    )
    {
        float overlayA = std::stoi(overlayColor.substr(0, 2), nullptr, 16) * VSCOpacity / 255;
        float overlayR = std::stoi(overlayColor.substr(2, 2), nullptr, 16);
        float overlayG = std::stoi(overlayColor.substr(4, 2), nullptr, 16);
        float overlayB = std::stoi(overlayColor.substr(6, 2), nullptr, 16);

        float baseA = std::stoi(baseColor.substr(0, 2), nullptr, 16) / 255.f;
        float baseR = std::stoi(baseColor.substr(2, 2), nullptr, 16);
        float baseG = std::stoi(baseColor.substr(4, 2), nullptr, 16);
        float baseB = std::stoi(baseColor.substr(6, 2), nullptr, 16);

        auto const overlayOpacity = overlayA / VSOpacity;
        auto const baseOpacity = (1 - overlayA / VSOpacity);

        return std::format("{:X}{:X}{:X}FF",
            static_cast<int>(std::clamp(overlayOpacity * overlayR + baseOpacity * baseA * baseR, 0.f, 255.f)),
            static_cast<int>(std::clamp(overlayOpacity * overlayG + baseOpacity * baseA * baseG, 0.f, 255.f)),
            static_cast<int>(std::clamp(overlayOpacity * overlayB + baseOpacity * baseA * baseB, 0.f, 255.f))
        );
    }

    std::string GetCompoundColor(
        std::string const& overlayColor,
        std::string const& baseColor,
        float VSOpacity,
        float VSCOpacity
    )
    {
        return getCompoundColorImpl(
            ReviseColor(overlayColor),
            ReviseColor(baseColor),
            VSOpacity,
            VSCOpacity
        );
    }
}
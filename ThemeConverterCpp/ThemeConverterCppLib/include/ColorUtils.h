#include <string>
namespace ThemeConverterCppLib
{
    std::string ReviseColor(std::string const& color);

    std::string GetCompoundColor(
        std::string const& overlayColor,
        std::string const& baseColor,
        float VSOpacity = 1.f,
        float VSCOpacity = 1.f
    );
}
#include <iostream>
#include <string>
#include <vector>
#include "ThemeConverter.h"

int main()
{
    ThemeConverterCppLib::Convert(R"(C:\Users\Peter\Desktop\cursortheme.jsonc)", R"(C:\Users\Peter\Desktop\test.vstheme)");
    return 0;
}
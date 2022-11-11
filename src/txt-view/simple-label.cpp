#include "simple-label.h"

ftxui::Component SimpleLabel(std::string &text)
{
    return ftxui::Renderer([&]
                           { return ftxui::text(text); });
}
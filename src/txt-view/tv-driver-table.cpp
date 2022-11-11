#include "tv-driver-table.h"

ftxui::Component TvDriverTable(std::vector<TvDriverTableEntrySP> &cars)
{
    using namespace ftxui;

    return ftxui::Renderer([&]
                           {                             
        ftxui::Elements nameElements({
            text("Name"),
        });
        ftxui::Elements currentCarElements({text("  ")});
        ftxui::Elements gaugeElements({text("Screen Time")});
        ftxui::Elements percentVElements({text("")});

            for (auto c : cars)
            {
                auto color1 = Color::Green;
                auto color2 = Color::DarkGreen;

                if (c->isCarStatic)
                {
                    color1 = Color::Red;
                    color2 = Color::DarkRed;
                }

                nameElements.push_back(color(color1, text(c->name)));
                currentCarElements.push_back(text(c->isCarSelected ? "**" : "  "));
                gaugeElements.push_back(color(color1, gauge(c->targetBarVal)));
                percentVElements.push_back(color(color1, text(c->targetStrVal)));

                nameElements.push_back(color(color2, text("")));
                currentCarElements.push_back(text("  "));
                gaugeElements.push_back(color(color2, gauge(c->actualBarVal)));
                percentVElements.push_back(color(color2, text(c->actualStrVal)));
            }

        ftxui::Elements e({
            hbox({
                vbox(nameElements),
                vbox(currentCarElements),
                vbox(gaugeElements) | flex,
                vbox(percentVElements),

            }) | border,
        });
        auto r = vbox(e);

        return r; });
}
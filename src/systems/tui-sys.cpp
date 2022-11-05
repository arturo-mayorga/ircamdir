#include "tui-sys.h"
#include "../components/car-comp.h"
#include "../components/cam-ctrl-comp.h"
#include "../components/app-state-comp.h"
#include "../components/session-comp.h"

#include <sstream>
#include <iomanip>

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

struct TuiDriverEntry
{
    std::string name;
    float targetBarVal;
    std::string targetStrVal;
    float actualBarVal;
    std::string actualStrVal;
    int isCarSelected;
    int isCarStatic;
};
typedef std::shared_ptr<TuiDriverEntry> TuiDriverEntrySP;

std::vector<TuiDriverEntrySP> getDriverEntries(class ECS::World *world)
{
    std::vector<TuiDriverEntrySP> ret;

    // figure out who has the current camera
    int currentCameraCarIdx = -1;
    world->each<CameraActualsComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<CameraActualsComponentSP> cStateH)
        {
            CameraActualsComponentSP cState = cStateH.get();
            currentCameraCarIdx = cState->currentCarIdx;
        });

    std::vector<std::pair<float, TuiDriverEntrySP>> screenTimeStrs;

    float maxScrPercent = 0;
    float maxTvPtsPercent = 0;

    world->each<BroadcastCarSummaryComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<BroadcastCarSummaryComponentSP> bStateH)
        {
            BroadcastCarSummaryComponentSP bState = bStateH.get();

            if (maxScrPercent < bState->scrTimePct)
            {
                maxScrPercent = bState->scrTimePct;
            }

            if (maxTvPtsPercent < bState->tvPtsPct)
            {
                maxTvPtsPercent = bState->tvPtsPct;
            }
        });
    float maxPctAxis = (maxScrPercent > maxTvPtsPercent) ? maxScrPercent : maxTvPtsPercent;

    world->each<StaticCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<StaticCarStateComponentSP> cStateH)
        {
            StaticCarStateComponentSP cState = cStateH.get();

            ECS::ComponentHandle<BroadcastCarSummaryComponentSP> bStateH = ent->get<BroadcastCarSummaryComponentSP>();
            ECS::ComponentHandle<DynamicCarStateComponentSP> dStateH = ent->get<DynamicCarStateComponentSP>();
            if (bStateH.isValid() && dStateH.isValid())
            {
                TuiDriverEntrySP entry(new TuiDriverEntry());
                int barSize = 0;

                BroadcastCarSummaryComponentSP bState = bStateH.get();
                DynamicCarStateComponentSP dState = dStateH.get();

                const int maxNameLen = 16;
                auto dispName = cState->name.substr(0, maxNameLen);

                entry->name = dispName;
                entry->isCarStatic = dState->deltaLapDistPct == 0;
                entry->targetBarVal = bState->tvPtsPct / maxPctAxis;
                {
                    std::stringstream sout;
                    sout << "  " << std::fixed << std::setprecision(2) << 100 * bState->tvPtsPct << "%";
                    entry->targetStrVal = sout.str();
                }

                entry->isCarSelected = (bState->idx == currentCameraCarIdx && currentCameraCarIdx != -1);

                entry->actualBarVal = bState->scrTimePct / maxPctAxis;
                {
                    std::stringstream sout;
                    sout << " " << std::fixed << std::setprecision(2) << 100 * bState->scrTimePct << "%";
                    entry->actualStrVal = sout.str();
                }

                // the "first" value is for sorting, we add 100 to the points just to push idle cars to the bottom
                screenTimeStrs.push_back(std::pair<float, TuiDriverEntrySP>((dState->deltaLapDistPct > 0) ? bState->tvPtsPct + 1000 : bState->tvPtsPct, entry));
            }
        });

    std::sort(screenTimeStrs.begin(), screenTimeStrs.end(), [](std::pair<float, TuiDriverEntrySP> &a, std::pair<float, TuiDriverEntrySP> &b)
              { return a.first > b.first; });

    for (auto p : screenTimeStrs)
    {
        ret.push_back(p.second);
    }

    return ret;
}

TuiSystem::~TuiSystem()
{
}

ftxui::Element renderHandler(class ECS::World *world)
{
    using namespace ftxui;
    ftxui::Elements nameElements({
        text("Name"),
    });
    ftxui::Elements currentCarElements({text("  ")});
    ftxui::Elements gaugeElements({text("Screen Time")});
    ftxui::Elements percentVElements({text("")});

    if (world)
    {
        auto cars = getDriverEntries(world);

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
    }

    std::string sessionNameStr("");
    world->each<SessionComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<SessionComponentSP> sCompH)
        {
            SessionComponentSP sComp = sCompH.get();
            std::stringstream sout;
            sout << "Session: " << sComp->num << " - " << sComp->name;
            sessionNameStr = sout.str();
        });

    ftxui::Elements e({
        text(sessionNameStr),
        hbox({
            vbox(nameElements),
            vbox(currentCarElements),
            vbox(gaugeElements) | flex,
            vbox(percentVElements),

        }) | border,
    });
    auto r = vbox(e);

    return r;
}

void TuiSystem::configure(class ECS::World *world)
{
    using namespace ftxui;
    static auto screen = ScreenInteractive::Fullscreen();

    static auto driverTable = Renderer([&]
                                       { 
    screen.PostEvent(Event::Custom); // post an event to make sure the ui responds to live data updates
    return renderHandler(_activeWorld); });

    static std::vector<std::string> entries = {
        "Closest Battle",
        "TV Point Fill",
        "Passive",
        "Incident Cam",
        "Leader Cam",
        "Exiting Cam",
    };

    static auto appModeDropDown = Dropdown(&entries, &_appMode);

    static auto layout = Container::Vertical({Renderer([]
                                                       { return text("App Mode:"); }),
                                              appModeDropDown,
                                              driverTable});

    _ftuiLoop = new Loop(&screen, layout);
}

void TuiSystem::unconfigure(class ECS::World *world)
{
    world->unsubscribeAll(this);

    delete _ftuiLoop;
}

void TuiSystem::tick(class ECS::World *world, float deltaTime)
{
    _activeWorld = world;

    world->each<ApplicationStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<ApplicationStateComponentSP> aStateH)
        {
            ApplicationStateComponentSP aState = aStateH.get();
            _appMode = aState->mode;
        });

    if (_ftuiLoop && !_ftuiLoop->HasQuitted())
    {
        _ftuiLoop->RunOnce();
    }
    else if (_ftuiLoop && _ftuiLoop->HasQuitted())
    {
        _isFinished = 1;
    }

    world->each<ApplicationStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<ApplicationStateComponentSP> aStateH)
        {
            ApplicationStateComponentSP aState = aStateH.get();
            aState->mode = (AppMode)_appMode;
        });
}

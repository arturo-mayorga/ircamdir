#include "tui-sys.h"
#include "../ecs-util.h"
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

std::vector<TvDriverTableEntrySP> getTvDriverEntries(class ECS::World *world)
{
    std::vector<TvDriverTableEntrySP> ret;

    // figure out who has the current camera
    int currentCameraCarIdx = -1;
    world->each<CameraActualsComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<CameraActualsComponentSP> cStateH)
        {
            CameraActualsComponentSP cState = cStateH.get();
            currentCameraCarIdx = cState->currentCarIdx;
        });

    std::vector<std::pair<float, TvDriverTableEntrySP>> screenTimeStrs;

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
                TvDriverTableEntrySP entry(new TvDriverTableEntry());
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
                screenTimeStrs.push_back(std::pair<float, TvDriverTableEntrySP>((dState->deltaLapDistPct > 0) ? bState->tvPtsPct + 1000 : bState->tvPtsPct, entry));
            }
        });

    std::sort(screenTimeStrs.begin(), screenTimeStrs.end(), [](std::pair<float, TvDriverTableEntrySP> &a, std::pair<float, TvDriverTableEntrySP> &b)
              { return a.first > b.first; });

    for (auto p : screenTimeStrs)
    {
        ret.push_back(p.second);
    }

    return ret;
}

ftxui::Component SimpleLabel(std::string &text)
{
    return ftxui::Renderer([&]
                           { return ftxui::text(text); });
}

TuiSystem::~TuiSystem() {}

void TuiSystem::configure(class ECS::World *world)
{
    using namespace ftxui;
    static auto screen = ScreenInteractive::Fullscreen();

    _dispModel.appMode = 0;

    _dispModel.appModeEntries = {
        "Closest Battle",
        "TV Point Fill",
        "Passive",
        "Incident Cam",
        "Leader Cam",
        "Exiting Cam",
    };

    auto appModeLabel = Renderer([]
                                 { 
                                    screen.PostEvent(Event::Custom); // post an event to make sure the ui responds to live data updates
                                    return text("App Mode:"); });

    static auto layout = Container::Vertical(
        {SimpleLabel(_dispModel.currentFrameInfo),
         Container::Horizontal({appModeLabel,
                                Dropdown(&_dispModel.appModeEntries, &_dispModel.appMode)}),
         SimpleLabel(_dispModel.sessionNameStr),
         TvDriverTable(_dispModel.tvDriverTableEntries)});

    _ftuiLoop = new Loop(&screen, layout);
}

void TuiSystem::unconfigure(class ECS::World *world)
{
    world->unsubscribeAll(this);

    delete _ftuiLoop;
}

void TuiSystem::tick(class ECS::World *world, float deltaTime)
{
    ApplicationStateComponentSP appStateComponent = ECSUtil::getFirstCmp<ApplicationStateComponentSP>(world);
    SessionComponentSP sessionComponent = ECSUtil::getFirstCmp<SessionComponentSP>(world);
    CameraActualsComponentSP cameraActualsComponent = ECSUtil::getFirstCmp<CameraActualsComponentSP>(world);

    _dispModel.appMode = appStateComponent->mode;

    _dispModel.tvDriverTableEntries = getTvDriverEntries(world);

    {
        std::stringstream sout;
        sout << "Session: " << sessionComponent->num << " - " << sessionComponent->name;
        _dispModel.sessionNameStr = sout.str();
    }

    {
        std::stringstream sout;
        sout << "Frame Num: " << cameraActualsComponent->replayFrameNum << "  Frame Num End: " << cameraActualsComponent->replayFrameNumEnd;
        _dispModel.currentFrameInfo = sout.str();
        ;
    }

    // update ui
    if (_ftuiLoop && !_ftuiLoop->HasQuitted())
    {
        _ftuiLoop->RunOnce();
    }
    else if (_ftuiLoop && _ftuiLoop->HasQuitted())
    {
        _isFinished = 1;
    }
    // end update ui

    appStateComponent->mode = (AppMode)(_dispModel.appMode);
}

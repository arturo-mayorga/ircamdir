#include "tui-sys.h"
#include "../ecs-util.h"
#include "../components/car-comp.h"
#include "../components/cam-ctrl-comp.h"
#include "../components/app-state-comp.h"
#include "../components/session-comp.h"

#include <sstream>
#include <iomanip>
#include <map>

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

ftxui::Component EventLogTable(std::vector<CarEventTableEntrySP> &overtakeLog, std::function<void()> on_click)
{
    using namespace ftxui;

    class Impl : public ComponentBase
    {
    public:
        Impl(std::vector<CarEventTableEntrySP> *overtakeLog,
             std::function<void()> on_click)
            : overtakeLog_(overtakeLog),
              on_click_(std::move(on_click)) {}

        // Component implementation:
        Element Render() override
        {
            ftxui::Elements nameElements({
                text("Name"),
            });
            ftxui::Elements noteElements({
                text("Notes"),
            });
            ftxui::Elements spacer1({text("  ")});
            ftxui::Elements spacer2({text("  ")});
            ftxui::Elements franeNumElements({text("Frame Num")});

            for (auto it = overtakeLog_->rbegin(); it != overtakeLog_->rend(); ++it)
            {
                auto ev = *it;
                auto color1 = Color::Green;
                auto color2 = Color::DarkGreen;

                nameElements.push_back(color(color1, text(ev->driverName)));
                noteElements.push_back(color(color1, text(ev->eventNote)));
                // spacer1.push_back(text("  "));

                std::stringstream sout;
                sout << ev->frameNumber;
                franeNumElements.push_back(color(color1, text(sout.str())));
            }

            ftxui::Elements e({
                hbox({
                    vbox(franeNumElements),
                    vbox(spacer1),
                    vbox(nameElements),
                    vbox(spacer2),
                    vbox(noteElements) | flex,

                }) | vscroll_indicator |
                    border,
            });
            auto r = vbox(e);

            return r | reflect(box_);
        }

        Decorator AnimatedColorStyle()
        {
            Decorator style = nothing;
            if (option_->animated_colors.background.enabled)
            {
                style = style | bgcolor(Color::Interpolate(
                                    animation_foreground_, //
                                    option_->animated_colors.background.inactive,
                                    option_->animated_colors.background.active));
            }
            if (option_->animated_colors.foreground.enabled)
            {
                style = style | color(Color::Interpolate(
                                    animation_foreground_, //
                                    option_->animated_colors.foreground.inactive,
                                    option_->animated_colors.foreground.active));
            }
            return style;
        }

        void SetAnimationTarget(float target)
        {
            if (option_->animated_colors.foreground.enabled)
            {
                animator_foreground_ =
                    animation::Animator(&animation_foreground_, target,
                                        option_->animated_colors.foreground.duration,
                                        option_->animated_colors.foreground.function);
            }
            if (option_->animated_colors.background.enabled)
            {
                animator_background_ =
                    animation::Animator(&animation_background_, target,
                                        option_->animated_colors.background.duration,
                                        option_->animated_colors.background.function);
            }
        }

        void OnAnimation(animation::Params &p) override
        {
            animator_background_.OnAnimation(p);
            animator_foreground_.OnAnimation(p);
        }

        void OnClick()
        {
            on_click_();
            animation_background_ = 0.5F; // NOLINT
            animation_foreground_ = 0.5F; // NOLINT
            SetAnimationTarget(1.F);      // NOLINT
        }

        bool OnEvent(Event event) override
        {
            if (event.is_mouse())
            {
                return OnMouseEvent(event);
            }

            if (event == Event::Return)
            {
                OnClick();
                return true;
            }
            return false;
        }

        bool OnMouseEvent(Event event)
        {
            mouse_hover_ =
                box_.Contain(event.mouse().x, event.mouse().y) && CaptureMouse(event);

            if (!mouse_hover_)
            {
                return false;
            }

            if (event.mouse().button == Mouse::Left &&
                event.mouse().motion == Mouse::Pressed)
            {
                TakeFocus();
                OnClick();
                return true;
            }

            return false;
        }

        bool Focusable() const final { return true; }

    private:
        std::vector<CarEventTableEntrySP> *overtakeLog_;
        std::function<void()> on_click_;
        bool mouse_hover_ = false;
        Box box_;
        Ref<ButtonOption> option_;
        float animation_background_ = 0;
        float animation_foreground_ = 0;
        animation::Animator animator_background_ =
            animation::Animator(&animation_background_);
        animation::Animator animator_foreground_ =
            animation::Animator(&animation_foreground_);
    };

    return Make<Impl>(&overtakeLog, std::move(on_click));
}

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

std::vector<CarEventTableEntrySP> getOvertakeEntries(class ECS::World *world)
{
    std::vector<CarEventTableEntrySP> ret;
    std::map<int, std::string> idx2name;
    std::map<int, float> idx2num;

    world->each<StaticCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<StaticCarStateComponentSP> cStateH)
        {
            StaticCarStateComponentSP cState = cStateH.get();
            idx2name[cState->idx] = cState->name;
        });

    world->each<DynamicCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
        {
            DynamicCarStateComponentSP cState = cStateH.get();
            idx2num[cState->idx] = cState->currentLap * 100 + cState->lapDistPct;
        });

    auto overtakeSummaryComponent = ECSUtil::getFirstCmp<OvertakeSummaryComponentSP>(world);

    for (auto ev : overtakeSummaryComponent->events)
    {
        CarEventTableEntrySP entry(new CarEventTableEntry());
        entry->frameNumber = ev->frameNumber;
        if (idx2name.count(ev->carIdx))
        {
            entry->driverName = idx2name[ev->carIdx];
        }
        else
        {
            entry->driverName = "bad name";
        }

        if (idx2name.count(ev->secCarIdx))
        {
            std::stringstream sout;
            sout << " overtakes -> " << idx2name[ev->secCarIdx];
            // sout << "  " << std::fixed << std::setprecision(8) << idx2num[ev->carIdx];
            entry->eventNote = sout.str();
        }
        else
        {
            entry->eventNote = "???";
        }

        ret.push_back(entry);
    }

    return ret;
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

    static int tab_selected;
    static std::vector<std::string> tab_values{
        "Screen Time",
        "Overtake Log",
        //"tab_3",
    };

    auto appModeLabel = Renderer([]
                                 { 
                                    screen.PostEvent(Event::Custom); // post an event to make sure the ui responds to live data updates
                                    return text("App Mode:"); });

    static auto layout = Container::Vertical(
        {
            SimpleLabel(_dispModel.currentFrameInfo),
            Container::Horizontal({appModeLabel,
                                   Dropdown(&_dispModel.appModeEntries, &_dispModel.appMode)}),
            SimpleLabel(_dispModel.sessionNameStr),
            Renderer([]
                     { return separator(); }),
            Toggle(&tab_values, &tab_selected),
            Container::Tab({
                               TvDriverTable(_dispModel.tvDriverTableEntries),
                               EventLogTable(_dispModel.overtakeLog, [&]
                                             { tab_selected = 0; }),
                           },
                           &tab_selected),
            //
        });

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

    _dispModel.overtakeLog = getOvertakeEntries(world);

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

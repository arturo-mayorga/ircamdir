#include "tui-sys.h"

#include "../ecs-util.h"
#include "../components/car-comp.h"
#include "../components/cam-ctrl-comp.h"
#include "../components/app-state-comp.h"
#include "../components/session-comp.h"

#include "ftxui/component/screen_interactive.hpp"

#include "../txt-view/simple-label.h"

#include <sstream>
#include <iomanip>
#include <map>
#include <memory>
#include <string>

std::map<int, std::string> getIdx2nameMap(class ECS::World *world)
{
    std::map<int, std::string> idx2name;
    world->each<StaticCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<StaticCarStateComponentSP> cStateH)
        {
            StaticCarStateComponentSP cState = cStateH.get();
            idx2name[cState->idx] = cState->name;
        });
    return idx2name;
}

std::map<int, float> getIdx2numMap(class ECS::World *world)
{
    std::map<int, float> idx2num;
    world->each<DynamicCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
        {
            DynamicCarStateComponentSP cState = cStateH.get();
            idx2num[cState->idx] = cState->currentLap * 100 + cState->lapDistPct;
        });
    return idx2num;
}

std::vector<CarEventTableEntrySP> getDetectedIncidetEntries(class ECS::World *world, std::set<int> &senDetectedIncidents, const std::map<int, std::string> &idx2name, const std::map<int, float> &idx2num)
{
    std::vector<CarEventTableEntrySP> ret;

    auto detectedIncidentSummaryComponent = ECSUtil::getFirstCmp<DetectedIncidentSummaryComponentSP>(world);
    auto cameraActualsComponent = ECSUtil::getFirstCmp<CameraActualsComponentSP>(world);

    for (auto ev : detectedIncidentSummaryComponent->events)
    {
        CarEventTableEntrySP entry(new CarEventTableEntry());
        entry->carIdx = ev->carIdx;
        entry->frameNumber = ev->frameNumber;
        if (idx2name.count(ev->carIdx))
        {
            entry->driverName = idx2name.find(ev->carIdx)->second;
        }
        else
        {
            entry->driverName = "bad name";
        }

        entry->eventNote = "Possible Incident";

        // sometimes we don't get the ext frame but if we get within the following
        // number of frames and we are looking at the correct car we'll make it count
        const int SEEN_FRAME_BUCKET_SIZE = 10;

        if (cameraActualsComponent->replayFrameNum / SEEN_FRAME_BUCKET_SIZE == entry->frameNumber / SEEN_FRAME_BUCKET_SIZE && ev->carIdx == cameraActualsComponent->currentCarIdx)
        {
            senDetectedIncidents.insert(entry->frameNumber / SEEN_FRAME_BUCKET_SIZE);
        }

        entry->seen = (senDetectedIncidents.count(entry->frameNumber / SEEN_FRAME_BUCKET_SIZE) != 0);

        ret.push_back(entry);
    }

    return ret;
}

extern std::string getEpochKey(CarEventSP event);
// {
//     const int frameBucketSize = 90 * 60;
//     const int pctDistBucketSize = 10;

//     int frameBucket = event->frameNumber / frameBucketSize;
//     int pctBucket = (100 * event->lapDistPct) / pctDistBucketSize;

//     std::stringstream sout;
//     sout << frameBucket << ":" << pctBucket;

//     return sout.str();
// }

std::vector<CarEventTableEntrySP> getOvertakeEntries(class ECS::World *world, std::set<int> &senOvertakes, const std::map<int, std::string> &idx2name, const std::map<int, float> &idx2num)
{
    std::vector<CarEventTableEntrySP> ret;

    auto overtakeSummaryComponent = ECSUtil::getFirstCmp<OvertakeSummaryComponentSP>(world);
    auto cameraActualsComponent = ECSUtil::getFirstCmp<CameraActualsComponentSP>(world);

    for (auto ev : overtakeSummaryComponent->events)
    {
        CarEventTableEntrySP entry(new CarEventTableEntry());
        entry->carIdx = ev->carIdx;
        entry->frameNumber = ev->frameNumber;
        entry->lap = ev->lapNumber;

        if (idx2name.count(ev->carIdx))
        {
            entry->driverName = idx2name.find(ev->carIdx)->second;
        }
        else
        {
            entry->driverName = "bad name";
        }

        if (idx2name.count(ev->secCarIdx))
        {
            std::stringstream sout;
            sout << getEpochKey(ev) << " overtakes -> " << idx2name.find(ev->secCarIdx)->second;
            entry->eventNote = sout.str();
        }
        else
        {
            entry->eventNote = "???";
        }

        // sometimes we don't get the ext frame but if we get within the following
        // number of frames and we are looking at the correct car we'll make it count
        const int SEEN_FRAME_BUCKET_SIZE = 10;

        if (cameraActualsComponent->replayFrameNum / SEEN_FRAME_BUCKET_SIZE == entry->frameNumber / SEEN_FRAME_BUCKET_SIZE && ev->carIdx == cameraActualsComponent->currentCarIdx)
        {
            senOvertakes.insert(entry->frameNumber / SEEN_FRAME_BUCKET_SIZE);
        }

        entry->seen = (senOvertakes.count(entry->frameNumber / SEEN_FRAME_BUCKET_SIZE) != 0);

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
        "Highlights"
        // "Incident Cam",
        // "Leader Cam",
        // "Exiting Cam"
    };

    static int tab_selected;
    static std::vector<std::string> tab_values{
        "Screen Time",
        "Overtake Log",
        "Incident Log"
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
                               CarEventLogTable(_dispModel.overtakeLog, [&](int frameNum, int carIdx) { // tab_selected = 0;
                                   _onEventLogClicked(frameNum, carIdx);
                               }),
                               CarEventLogTable(_dispModel.detectedIncidentLog, [&](int frameNum, int carIdx) { // tab_selected = 0;
                                   _onEventLogClicked(frameNum, carIdx);
                               }),
                           },
                           &tab_selected),
            //
        });

    _ftuiLoop = new Loop(&screen, layout);
}

void TuiSystem::_onEventLogClicked(int frameNum, int carIdx)
{
    _dispModel.world->emit<OnCameraChangeRequest>(OnCameraChangeRequest(carIdx));
    _dispModel.world->emit<OnFrameNumChangeRequest>(OnFrameNumChangeRequest(frameNum - 5 * 60));
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

    std::map<int, std::string> idx2name = getIdx2nameMap(world);
    std::map<int, float> idx2num = getIdx2numMap(world);

    _dispModel.world = world;
    _dispModel.appMode = appStateComponent->mode;

    _dispModel.tvDriverTableEntries = getTvDriverEntries(world);

    _dispModel.overtakeLog = getOvertakeEntries(world, _seenOvertakes, idx2name, idx2num);
    _dispModel.detectedIncidentLog = getDetectedIncidetEntries(world, _seenDetectedIncidents, idx2name, idx2num);

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

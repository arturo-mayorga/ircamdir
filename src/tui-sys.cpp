#include "tui-sys.h"
#include "car-comp.h"
#include "cam-ctrl-comp.h"
#include "app-state-comp.h"
#include "session-comp.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <string>

#define REFRESH_DELTA 3000

TuiSystem::~TuiSystem()
{
}

void TuiSystem::configure(class ECS::World *world)
{
}

void TuiSystem::unconfigure(class ECS::World *world)
{
    world->unsubscribeAll(this);
}

void TuiSystem::tick(class ECS::World *world, float deltaTime)
{
    static float timeSinceLastUpdate = REFRESH_DELTA + 1;
    static AppMode lastAppMode = AppMode::PASSIVE;
    AppMode currentAppMode = AppMode::PASSIVE;

    world->each<ApplicationStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<ApplicationStateComponentSP> aStateH)
        {
            ApplicationStateComponentSP aState = aStateH.get();
            currentAppMode = aState->mode;
        });

    timeSinceLastUpdate += deltaTime;

    if (timeSinceLastUpdate > REFRESH_DELTA || currentAppMode != lastAppMode)
    {
        this->_drawScreen(world);
        timeSinceLastUpdate = 0;
    }

    lastAppMode = currentAppMode;
}

void drawPercentBar(float percent, float maxPercent, int strSize, std::stringstream &sout, char fillChar)
{
    sout << "[";
    for (float i = 0; i < strSize; ++i)
    {
        sout << ((i / strSize < percent / maxPercent) ? fillChar : ' ');
    }
    sout << "] ";
    sout << std::fixed << std::setprecision(2) << 100 * percent << "%";
}

void _drawAppStateControls(class ECS::World *world)
{
    static int first = 1;
    static std::string appModeNames[AppMode::MODE_COUNT];

    if (first)
    {
        first = 0;
        appModeNames[AppMode::PASSIVE] = "PASSIVE       ";
        appModeNames[AppMode::TV_POINT_FILL] = "TV POINT FILL ";
        appModeNames[AppMode::CLOSEST_BATTLE] = "CLOSEST BATTLE";
    }
    AppMode currentMode = AppMode::PASSIVE;
    world->each<ApplicationStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<ApplicationStateComponentSP> aStateH)
        {
            ApplicationStateComponentSP aState = aStateH.get();
            currentMode = aState->mode;
        });

    std::cout << std::endl
              << "    Current App Mode: " << appModeNames[currentMode] << std::endl;

    if (currentMode != AppMode::PASSIVE)
    {
        std::cout << "    [Spacebar: enter passive mode]";
    }
    else
    {
        std::cout << "    [Spacebar: exit Passive mode]";
    }

    std::cout << "  [b: Closest Battle]  [t: TV Point Fill]" << std::endl;
}

void TuiSystem::_drawScreen(class ECS::World *world)
{
    const int scrRows = 60;
    const int scrCols = 80;

    std::cout << std::endl
              << std::endl
              << std::endl
              << std::endl;

    // figure out who has the current camera
    int currentCameraTarget = -1;
    int currentCameraCarIdx = 0;
    world->each<CameraControlComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<CameraControlComponentSP> cStateH)
        {
            CameraControlComponentSP cState = cStateH.get();
            currentCameraTarget = cState->targetCarPosActual;
        });

    world->each<DynamicCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
        {
            DynamicCarStateComponentSP cState = cStateH.get();
            if (currentCameraTarget == cState->officialPos)
            {
                currentCameraCarIdx = cState->idx;
            }
        });

    std::string sessionNameStr("");
    world->each<SessionComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<SessionComponentSP> sCompH)
        {
            SessionComponentSP sComp = sCompH.get();
            std::stringstream sout;
            sout << sComp->num << " - " << sComp->name;
            sessionNameStr = sout.str();
        });
    std::cout << "Current Session: " << sessionNameStr << std::endl
              << std::endl;

    std::vector<std::pair<float, std::string>> screenTimeStrs;

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

    std::cout << "Name                Screen Time" << std::endl;

    world->each<StaticCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<StaticCarStateComponentSP> cStateH)
        {
            StaticCarStateComponentSP cState = cStateH.get();

            ECS::ComponentHandle<BroadcastCarSummaryComponentSP> bStateH = ent->get<BroadcastCarSummaryComponentSP>();
            if (bStateH.isValid())
            {
                std::stringstream sout;
                int barSize = 0;

                BroadcastCarSummaryComponentSP bState = bStateH.get();

                const int maxNameLen = 16;
                auto dispName = cState->name.substr(0, maxNameLen);
                sout << dispName;

                for (int i = (int)dispName.length(); i < maxNameLen; ++i)
                {
                    sout << " ";
                }

                sout << ((bState->idx == currentCameraCarIdx) ? " ** target: " : "    target: ");
                barSize = scrCols - maxNameLen - /*size of label "    actual: "*/ 17;
                drawPercentBar(bState->tvPtsPct, maxPctAxis, barSize, sout, 'X');

                sout << std::endl;

                for (int i = 0; i < maxNameLen; ++i)
                {
                    sout << " ";
                }

                sout << "    actual: ";
                barSize = scrCols - maxNameLen - /*size of label "    actual: "*/ 17;
                drawPercentBar(bState->scrTimePct, maxPctAxis, barSize, sout, '-');

                sout << std::endl;

                screenTimeStrs.push_back(std::pair<float, std::string>(bState->tvPtsPct, sout.str()));

                // std::cout << sout.str();
            }
        });

    std::sort(screenTimeStrs.begin(), screenTimeStrs.end(), [](std::pair<float, std::string> &a, std::pair<float, std::string> &b)
              { return a.first > b.first; });

    for (auto p : screenTimeStrs)
    {
        std::cout << p.second;
    }

    _drawAppStateControls(world);
}
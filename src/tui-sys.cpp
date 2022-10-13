#include "tui-sys.h"
#include "car-comp.h"
#include "cam-ctrl-comp.h"

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

    timeSinceLastUpdate += deltaTime;

    if (timeSinceLastUpdate > REFRESH_DELTA)
    {
        this->_drawScreen(world);
        timeSinceLastUpdate = 0;
    }
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

    float totalTime = 0;
    float maxScrTime = 0;
    float totalTvPts = 0;
    float maxTvPts = 0;

    std::vector<std::pair<float, std::string>> screenTimeStrs;

    world->each<BroadcastCarInfoComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<BroadcastCarInfoComponentSP> bStateH)
        {
            BroadcastCarInfoComponentSP bState = bStateH.get();
            totalTime += bState->scrTime;
            totalTvPts += bState->tvPoints;

            if (maxScrTime < bState->scrTime)
            {
                maxScrTime = bState->scrTime;
            }

            if (maxTvPts < bState->tvPoints)
            {
                maxTvPts = bState->tvPoints;
            }
        });
    float maxScrPercent = maxScrTime / totalTime;
    float maxTvPtsPercent = maxTvPts / totalTvPts;
    float maxPctAxis = (maxScrPercent > maxTvPtsPercent) ? maxScrPercent : maxTvPtsPercent;

    std::cout << "Name                Screen Time" << std::endl;

    world->each<StaticCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<StaticCarStateComponentSP> cStateH)
        {
            StaticCarStateComponentSP cState = cStateH.get();

            ECS::ComponentHandle<BroadcastCarInfoComponentSP> bStateH = ent->get<BroadcastCarInfoComponentSP>();
            if (bStateH.isValid())
            {
                std::stringstream sout;
                int barSize = 0;

                BroadcastCarInfoComponentSP bState = bStateH.get();

                const int maxNameLen = 16;
                auto dispName = cState->name.substr(0, maxNameLen);
                sout << dispName;

                for (int i = (int)dispName.length(); i < maxNameLen; ++i)
                {
                    sout << " ";
                }

                sout << ((bState->idx == currentCameraCarIdx) ? " ** target: " : "    target: ");
                barSize = scrCols - maxNameLen - /*size of label "    actual: "*/ 17;
                drawPercentBar(bState->tvPoints / totalTvPts, maxPctAxis, barSize, sout, 'X');

                sout << std::endl;

                for (int i = 0; i < maxNameLen; ++i)
                {
                    sout << " ";
                }

                sout << "    actual: ";
                barSize = scrCols - maxNameLen - /*size of label "    actual: "*/ 17;
                drawPercentBar(bState->scrTime / totalTime, maxPctAxis, barSize, sout, '-');

                sout << std::endl;

                screenTimeStrs.push_back(std::pair<float, std::string>(bState->tvPoints, sout.str()));

                // std::cout << sout.str();
            }
        });

    std::sort(screenTimeStrs.begin(), screenTimeStrs.end(), [](std::pair<float, std::string> &a, std::pair<float, std::string> &b)
              { return a.first > b.first; });

    for (auto p : screenTimeStrs)
    {
        std::cout << p.second;
    }
}
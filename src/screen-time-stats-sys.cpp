#include "screen-time-stats-sys.h"
#include "car-comp.h"
#include "cam-ctrl-comp.h"

#include <iostream>
#include <iomanip>
#include <map>

void recordTimeByPosition(int topN, std::map<int, float> &mapStore, float deltaTime, DynamicCarStateComponentSP &cState)
{
    int cIdx = cState->idx;
    if (mapStore.count(cIdx) == 0)
    {
        mapStore[cIdx] = 0;
    }

    if (cState->officialPos <= topN)
    {
        mapStore[cIdx] += deltaTime;
    }
}

ScreenTimeStatsSys::~ScreenTimeStatsSys()
{
}

void ScreenTimeStatsSys::configure(class ECS::World *world)
{
}

void ScreenTimeStatsSys::unconfigure(class ECS::World *world)
{
    world->unsubscribeAll(this);
}

void ScreenTimeStatsSys::tick(class ECS::World *world, float deltaTime)
{
    static std::map<int, float> scrTimeByCarIdx;
    static std::map<int, float> leadTimeByCarIdx;
    static std::map<int, float> top3TimeByCarIdx;
    static std::map<int, float> top10TimeByCarIdx;
    static std::map<int, float> top20TimeByCarIdx;

    int currentCameraTarget = -1;
    world->each<CameraControlComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<CameraControlComponentSP> cStateH)
        {
            CameraControlComponentSP cState = cStateH.get();
            currentCameraTarget = cState->targetCarPosActual;
        });

    int currentCarIndex = -1;
    world->each<DynamicCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
        {
            DynamicCarStateComponentSP cState = cStateH.get();

            int cIdx = cState->idx;

            if (scrTimeByCarIdx.count(cIdx) == 0)
            {
                scrTimeByCarIdx[cIdx] = 0;
            }

            if (currentCameraTarget == cState->officialPos)
            {
                float cTime = 0;
                scrTimeByCarIdx[cIdx] += deltaTime;
            }

            recordTimeByPosition(1, leadTimeByCarIdx, deltaTime, cState);
            recordTimeByPosition(3, top3TimeByCarIdx, deltaTime, cState);
            recordTimeByPosition(10, top10TimeByCarIdx, deltaTime, cState);
            recordTimeByPosition(20, top20TimeByCarIdx, deltaTime, cState);
        });

    static float timeSinceLastReport = 0;
    timeSinceLastReport += deltaTime;

    if (timeSinceLastReport > 20000)
    {
        timeSinceLastReport = 0;
        float totalT = 0;

        for (auto time : scrTimeByCarIdx)
        {
            totalT += time.second;
        }

        for (auto time : scrTimeByCarIdx)
        {
            std::cout << "carIdx: " << time.first << "    cam time: " << std::fixed << std::setprecision(2) << 100 * time.second / totalT << "%"
                      << "    t1 time: " << 100 * leadTimeByCarIdx[time.first] / totalT << "    t3 time: " << 100 * top3TimeByCarIdx[time.first] / totalT << "    t10 time: " << 100 * top10TimeByCarIdx[time.first] / totalT << "    t20 time: " << 100 * top20TimeByCarIdx[time.first] / totalT << std::endl;
        }
    }
}

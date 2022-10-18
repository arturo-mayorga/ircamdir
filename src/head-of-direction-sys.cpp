#include "head-of-direction-sys.h"
#include "cam-ctrl-comp.h"
#include "app-state-comp.h"

HeadOfDirectionSystem::~HeadOfDirectionSystem() {}

void HeadOfDirectionSystem::configure(class ECS::World *world) {}

void HeadOfDirectionSystem::unconfigure(class ECS::World *world) {}

void HeadOfDirectionSystem::tick(class ECS::World *world, float deltaTime)
{
    static float appModeTime[AppMode::MODE_COUNT];
    static int first = 1;

    if (first)
    {
        first = 0;
        for (int i = 0; i < AppMode::MODE_COUNT; ++i)
        {
            appModeTime[i] = 0.0f;
        }
    }
    AppMode currentAppMode = AppMode::PASSIVE;

    world->each<ApplicationStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<ApplicationStateComponentSP> cStateH)
        {
            currentAppMode = cStateH.get()->mode;
        });

    world->each<CameraControlComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<CameraControlComponentSP> cStateH)
        {
            CameraControlComponentSP cState = cStateH.get();
            switch (currentAppMode)
            {
            case AppMode::CLOSEST_BATTLE:
                cState->targetCarPosRequested = cState->closestBattleTarget;
                break;
            case AppMode::TV_POINT_FILL:
                cState->targetCarPosRequested = cState->tvPointsTarget;
                break;
            }
        });

    appModeTime[currentAppMode] += deltaTime;
}

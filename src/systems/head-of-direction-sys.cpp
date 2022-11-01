#include "head-of-direction-sys.h"
#include "../components/cam-ctrl-comp.h"
#include "../components/app-state-comp.h"

HeadOfDirectionSystem::~HeadOfDirectionSystem() {}

void HeadOfDirectionSystem::configure(class ECS::World *world) {}

void HeadOfDirectionSystem::unconfigure(class ECS::World *world) {}

void HeadOfDirectionSystem::tick(class ECS::World *world, float deltaTime)
{
    static float appModeTime[AppMode::MODE_COUNT];
    static int first = 1;
    static AppMode prevAppMode = AppMode::PASSIVE;

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

    world->each<CameraDirectionSubTargetsComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<CameraDirectionSubTargetsComponentSP> bStateH)
        {
            ECS::ComponentHandle<CameraActualsComponentSP> aStateH = ent->get<CameraActualsComponentSP>();

            if (bStateH.isValid() && aStateH.isValid())
            {
                CameraDirectionSubTargetsComponentSP bState = bStateH.get();
                CameraActualsComponentSP aState = aStateH.get();

                if ((aState->timeSinceLastChange > 10000 || prevAppMode != currentAppMode) && currentAppMode != AppMode::PASSIVE)
                {
                    int targetCarIdx;
                    switch (currentAppMode)
                    {
                    case AppMode::CLOSEST_BATTLE:
                        targetCarIdx = bState->closestBattleCarIdx;
                        break;
                    case AppMode::TV_POINT_FILL:
                        targetCarIdx = bState->tvPointsCarIdx;
                        break;
                    case AppMode::EXITING_CAM:
                        targetCarIdx = SpecialCarNum::EXITING;
                        break;
                    case AppMode::INCIDENT_CAM:
                        targetCarIdx = SpecialCarNum::INCIDENT;
                        break;
                    case AppMode::LEADER_CAM:
                        targetCarIdx = SpecialCarNum::LEADER;
                        break;
                    }

                    world->emit<OnCameraChangeRequest>(OnCameraChangeRequest(targetCarIdx));
                }
            }
        });

    prevAppMode = currentAppMode;

    appModeTime[currentAppMode] += deltaTime;
}

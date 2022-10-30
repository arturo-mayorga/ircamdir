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

    world->each<CameraRequestComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<CameraRequestComponentSP> cStateH)
        {
            CameraRequestComponentSP cState = cStateH.get();

            ECS::ComponentHandle<CameraDirectionSubTargetsComponentSP> bStateH = ent->get<CameraDirectionSubTargetsComponentSP>();
            ECS::ComponentHandle<CameraActualsComponentSP> aStateH = ent->get<CameraActualsComponentSP>();

            if (bStateH.isValid() && aStateH.isValid())
            {
                CameraDirectionSubTargetsComponentSP bState = bStateH.get();
                CameraActualsComponentSP aState = aStateH.get();

                switch (currentAppMode)
                {
                case AppMode::CLOSEST_BATTLE:
                    cState->targetCarIdx = bState->closestBattleCarIdx;
                    break;
                case AppMode::TV_POINT_FILL:
                    cState->targetCarIdx = bState->tvPointsCarIdx;
                    break;
                case AppMode::EXITING_CAM:
                    cState->targetCarIdx = SpecialCarNum::EXITING;
                    break;
                case AppMode::INCIDENT_CAM:
                    cState->targetCarIdx = SpecialCarNum::INCIDENT;
                    break;
                case AppMode::LEADER_CAM:
                    cState->targetCarIdx = SpecialCarNum::LEADER;
                    break;
                }

                cState->changeThisFrame = ((aState->timeSinceLastChange > 10000 || prevAppMode != currentAppMode) && currentAppMode != AppMode::PASSIVE) ? 1 : 0;
            }
        });

    prevAppMode = currentAppMode;

    appModeTime[currentAppMode] += deltaTime;
}

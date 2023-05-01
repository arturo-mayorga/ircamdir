#include "../ecs-util.h"

#include "head-of-direction-sys.h"
#include "../components/cam-ctrl-comp.h"
#include "../components/app-state-comp.h"
#include "../components/car-comp.h"

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
                    auto emitCameraRequest = true;
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
                    case AppMode::HIGHLIGHT_REP:
                        tickHighlight(world, deltaTime, prevAppMode != AppMode::HIGHLIGHT_REP);
                        emitCameraRequest = false;
                        break;
                    }

                    if (emitCameraRequest)
                    {
                        world->emit<OnCameraChangeRequest>(OnCameraChangeRequest(targetCarIdx));
                    }
                }
            }
        });

    prevAppMode = currentAppMode;

    appModeTime[currentAppMode] += deltaTime;
}

void HeadOfDirectionSystem::tickHighlight(class ECS::World *world, float deltaTime, bool startThisFrame)
{
    const int starting = 0;
    const int transitioning = 1;
    const int watching = 2;
    const int triggerTransition = 3;

    static int currentEntry = 0;
    static int currentState = starting;

    if (startThisFrame)
    {
        currentState = starting;
        currentEntry = 0;
    }

    auto cameraActualsComponent = ECSUtil::getFirstCmp<CameraActualsComponentSP>(world);
    auto overtakeSummaryComponent = ECSUtil::getFirstCmp<OvertakeSummaryComponentSP>(world);

    auto currentFrame = cameraActualsComponent->replayFrameNum;

    if (overtakeSummaryComponent->events.size() <= currentEntry)
    {
        ApplicationStateComponentSP appStateComponent = ECSUtil::getFirstCmp<ApplicationStateComponentSP>(world);
        appStateComponent->mode = AppMode::TV_POINT_FILL;
        return;
    }

    auto targetEvent = overtakeSummaryComponent->events[currentEntry];

    auto targetWindowStart = targetEvent->frameNumber - 5 * 60;
    auto targetWindowEnd = targetEvent->frameNumber + 2 * 60;
    auto carIdx = targetEvent->carIdx;

    bool currentlyInWindow = currentFrame >= targetWindowStart && currentFrame <= targetWindowEnd;

    auto secondsToWindow = (currentFrame - targetWindowEnd) / 60;

    if (starting == currentState)
    {
        // go to target
        world->emit<OnCameraChangeRequest>(OnCameraChangeRequest(carIdx));
        world->emit<OnFrameNumChangeRequest>(OnFrameNumChangeRequest(targetWindowStart));

        currentState = transitioning;
    }
    else if (transitioning == currentState)
    {
        if (currentlyInWindow)
        {
            currentState = watching;
        }
    }
    else if (watching == currentState)
    {
        if (!currentlyInWindow)
        {
            currentState = triggerTransition;
            ++currentEntry;
        }
    }
    else if (triggerTransition == currentState)
    {
        world->emit<OnCameraChangeRequest>(OnCameraChangeRequest(carIdx));
        world->emit<OnFrameNumChangeRequest>(OnFrameNumChangeRequest(targetWindowStart));
        currentState = transitioning;
    }
}

#include "overtake-detector-system.h"
#include "../components/car-comp.h"
#include "../components/cam-ctrl-comp.h"

#include "../ecs-util.h"

#include <map>

#define MIN_PCT_DELTA 0.005

OvertakeDetectorSystem::~OvertakeDetectorSystem() {}
void OvertakeDetectorSystem::configure(class ECS::World *world) {}
void OvertakeDetectorSystem::unconfigure(class ECS::World *world) {}

void OvertakeDetectorSystem::tick(class ECS::World *world, float deltaTime)
{
    std::vector<DynamicCarStateComponentSP> states;

    static std::map<int, int> carIdx2rtPos;

    world->each<DynamicCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
        {
            DynamicCarStateComponentSP cState = cStateH.get();

            if (cState->isInPits == 0 && cState->deltaLapDistPct * 100000 > MIN_PCT_DELTA)
            {
                // ignore cars if they are in the pits or moving too slow
                states.push_back(cState);
            }
        });

    // sort the states by track position and current lap
    std::sort(states.begin(), states.end(), [](DynamicCarStateComponentSP &a, DynamicCarStateComponentSP &b)
              { return (a->currentLap == b->currentLap) ? a->lapDistPct > b->lapDistPct : a->currentLap > b->currentLap; });

    auto overtakeSummaryComponent = ECSUtil::getFirstCmp<OvertakeSummaryComponentSP>(world);
    auto cameraActualsComponent = ECSUtil::getFirstCmp<CameraActualsComponentSP>(world);

    std::vector<CarEventSP> newEvents;
    int secCar = -1;
    int i = 0;
    for (auto c : states)
    {
        if (!carIdx2rtPos.count(c->idx))
        {
            carIdx2rtPos[c->idx] = i;
        }

        if (carIdx2rtPos[c->idx] > i)
        {
            // overtaking car detected
            CarEventSP ev(new CarEvent());

            ev->carIdx = c->idx;
            ev->frameNumber = cameraActualsComponent->replayFrameNum;

            newEvents.push_back(ev);
        }
        else if (carIdx2rtPos[c->idx] < i)
        {
            // overtaked car
            secCar = c->idx;
        }

        carIdx2rtPos[c->idx] = i;

        ++i;
    }

    if (newEvents.size() == 1 && secCar >= 0)
    {
        // the only likely reason you'd have more than one event per frame
        // is if someone towed... just ignore that.
        auto ev = newEvents[0];
        ev->secCarIdx = secCar;
        overtakeSummaryComponent->events.push_back(ev);
    }
}
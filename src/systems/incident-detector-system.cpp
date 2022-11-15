#include "incident-detector-system.h"
#include "../components/car-comp.h"
#include <iostream>
#include <map>

#define EV_BOCKET_SIZE 600
#define NUM_OF_EVENTS_IN_WINDOW 3

struct EvenBucket
{
    int firstFrame;
    int lastFrame;
    int countinWindow;
};
typedef std::shared_ptr<EvenBucket> EvenBucketSP;

IncidentDetectorSystem::~IncidentDetectorSystem() {}
void IncidentDetectorSystem::configure(class ECS::World *world) {}
void IncidentDetectorSystem::unconfigure(class ECS::World *world) {}

void IncidentDetectorSystem::tick(class ECS::World *world, float deltaTime)
{
    auto overtakeComp = ECSUtil::getFirstCmp<OvertakeSummaryComponentSP>(world);
    auto incidentComp = ECSUtil::getFirstCmp<DetectedIncidentSummaryComponentSP>(world);

    std::map<int, EvenBucketSP> caridx2eventBucket;

    incidentComp->events.clear();

    for (auto oev : overtakeComp->events)
    {
        if (caridx2eventBucket.count(oev->secCarIdx))
        {
            auto eb = caridx2eventBucket[oev->secCarIdx];

            if (oev->frameNumber - eb->lastFrame < EV_BOCKET_SIZE)
            {
                eb->lastFrame = oev->frameNumber;
                eb->countinWindow += 1;

                if (eb->countinWindow == NUM_OF_EVENTS_IN_WINDOW)
                {
                    CarEventSP nev(new CarEvent());
                    nev->carIdx = oev->secCarIdx;
                    nev->frameNumber = eb->firstFrame;
                    nev->secCarIdx = -1;
                    incidentComp->events.push_back(nev);
                }
            }
            else
            {
                eb->countinWindow = 1;
                eb->firstFrame = oev->frameNumber;
                eb->lastFrame = oev->frameNumber;
            }
        }
        else
        {
            EvenBucketSP eb(new EvenBucket());
            eb->countinWindow = 1;
            eb->firstFrame = oev->frameNumber;
            eb->lastFrame = oev->frameNumber;
            caridx2eventBucket[oev->secCarIdx] = eb;
        }
    }
}

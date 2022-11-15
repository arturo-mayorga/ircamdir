#include "ecs.h"
#include <iostream>

#include <Windows.h>
#include <sysinfoapi.h>

#include "systems/irtelemetry-sys.h"
#include "systems/overtake-detector-system.h"
#include "systems/incident-detector-system.h"
#include "systems/closest-battle-director-sys.h"
#include "systems/broadcast-car-info-collector-sys.h"
#include "systems/tui-sys.h"
#include "systems/broadcast-summary-sys.h"
#include "systems/tv-point-director-sys.h"
#include "systems/head-of-direction-sys.h"

#include "components/car-comp.h"
#include "components/cam-ctrl-comp.h"

#include "components/app-state-comp.h"
#include "components/session-comp.h"

#include <ctime>

int main()
{
    ECS::World *world = ECS::World::createWorld();

    world->registerSystem(new IrTelemetrySystem());
    world->registerSystem(new OvertakeDetectorSystem());
    world->registerSystem(new IncidentDetectorSystem());
    world->registerSystem(new BroadcastCarInfoCollectorSystem());
    world->registerSystem(new BroadcastSummarySystem());
    world->registerSystem(new TvPointDirectorSystem());
    world->registerSystem(new ClosestBattleDirectorSystem());
    world->registerSystem(new HeadOfDirectionSystem());
    TuiSystem *tui = new TuiSystem();
    world->registerSystem(tui);

    ECS::Entity *ent = world->create();
    ent->assign<CameraActualsComponentSP>(new CameraActualsComponent());
    ent->assign<CameraDirectionSubTargetsComponentSP>(new CameraDirectionSubTargetsComponent());
    ent->assign<ApplicationStateComponentSP>(new ApplicationStateComponent());
    ent->assign<SessionComponentSP>(new SessionComponent());
    ent->assign<OvertakeSummaryComponentSP>(new OvertakeSummaryComponent());
    ent->assign<DetectedIncidentSummaryComponentSP>(new DetectedIncidentSummaryComponent());

    auto tPrev = GetTickCount();

    while (!tui->isFinished())
    {
        auto t = GetTickCount();
        world->tick((float)(t - tPrev));
        Sleep(10);
        tPrev = t;
    }

    world->destroyWorld();

    return 0;
}
#include "ecs.h"
#include <iostream>

#include <Windows.h>
#include <sysinfoapi.h>

#include "systems/irtelemetry-sys.h"
#include "systems/closest-battle-director-sys.h"
#include "systems/broadcast-car-info-collector-sys.h"
#include "systems/tui-sys.h"
#include "systems/console-kb-sys.h"
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
    world->registerSystem(new BroadcastCarInfoCollectorSystem());
    world->registerSystem(new BroadcastSummarySystem());
    world->registerSystem(new TvPointDirectorSystem());
    world->registerSystem(new ClosestBattleDirectorSystem());
    world->registerSystem(new HeadOfDirectionSystem());
    world->registerSystem(new TuiSystem());
    world->registerSystem(new ConsoleKbSystem());

    ECS::Entity *ent = world->create();
    ent->assign<CameraRequestComponentSP>(new CameraRequestComponent());
    ent->assign<CameraActualsComponentSP>(new CameraActualsComponent());
    ent->assign<CameraDirectionSubTargetsComponentSP>(new CameraDirectionSubTargetsComponent());
    ent->assign<ApplicationStateComponentSP>(new ApplicationStateComponent());
    ent->assign<SessionComponentSP>(new SessionComponent());

    std::cout << "Application Start" << std::endl
              << "==========================" << std::endl;

    auto tPrev = GetTickCount();

    for (;;)
    {
        auto t = GetTickCount();
        world->tick((float)(t - tPrev));
        Sleep(10);
        tPrev = t;
    }

    return 0;
}
#include "ecs.h"
#include <iostream>

#include <Windows.h>
#include <sysinfoapi.h>

#include "systems/irtelemetry-sys.h"
#include "systems/closest-battle-director-sys.h"
#include "systems/screen-time-stats-sys.h"
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

class TestSystem : public ECS::EntitySystem
{
public:
    virtual ~TestSystem() {}

    virtual void configure(class ECS::World *world) override
    {
    }

    virtual void unconfigure(class ECS::World *world) override
    {
        world->unsubscribeAll(this);
    }

    virtual void tick(class ECS::World *world, float deltaTime) override
    {
        // std::cout << "TestSystem::tick" << std::endl;
    }
};

int main()
{
    ECS::World *world = ECS::World::createWorld();

    world->registerSystem(new TestSystem());
    world->registerSystem(new IrTelemetrySystem());
    world->registerSystem(new ClosestBattleDirectorSystem());
    world->registerSystem(new ScreenTimeStatsSys());
    world->registerSystem(new TuiSystem());
    world->registerSystem(new ConsoleKbSystem());
    world->registerSystem(new BroadcastSummarySystem());
    world->registerSystem(new TvPointDirectorSystem());
    world->registerSystem(new HeadOfDirectionSystem());

    ECS::Entity *ent = world->create();
    ent->assign<CameraControlComponentSP>(new CameraControlComponent());
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
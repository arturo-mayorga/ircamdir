#include "ecs.h"
#include <iostream>

#include <Windows.h>
#include <sysinfoapi.h>

#include "irtelemetry-sys.h"
#include "closest-battle-director-sys.h"
#include "screen-time-stats-sys.h"
#include "tui-sys.h"
#include "console-kb-sys.h"
#include "broadcast-summary-sys.h"
#include "tv-point-director-sys.h"
#include "head-of-direction-sys.h"

#include "car-comp.h"
#include "cam-ctrl-comp.h"

#include "app-state-comp.h"
#include "session-comp.h"

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
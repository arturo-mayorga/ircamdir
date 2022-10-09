#include "ecs.h"
#include <iostream>

#include <Windows.h>
#include <sysinfoapi.h>

#include "irtelemetry-sys.h"
#include "battle-detect-sys.h"

#include "car-comp.h"
#include "cam-ctrl-comp.h"

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

    ECS::EntitySystem *testSystem = world->registerSystem(new TestSystem());
    ECS::EntitySystem *gamepadSystem = world->registerSystem(new IrTelemetrySystem());
    ECS::EntitySystem *battleDetectSystem = world->registerSystem(new BattleDetectSystem());

    ECS::Entity *ent = world->create();
    auto camCtrlCmp = ent->assign<CameraControlComponentSP>(new CameraControlComponent());

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
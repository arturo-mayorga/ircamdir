#include "ecs.h"
#include <iostream>

#include <Windows.h>

#include "irtelemetry-sys.h"

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

    // ECS::Entity *ent = world->create();
    // auto gamepadState = ent->assign<GamepadStateComponent>();

    std::cout << "Application Start" << std::endl
              << "==========================" << std::endl;

    for (;;)
    {

        world->tick(10.f);
        Sleep(10);
    }

    return 0;
}
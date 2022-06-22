#include "ecs.h"
#include <iostream>
#include "gamepad-sys.h"

#include <Windows.h>

class TestSystem : public ECS::EntitySystem
{
public:
	virtual ~TestSystem() {}

	virtual void configure(class ECS::World* world) override
	{
	}

	virtual void unconfigure(class ECS::World* world) override
	{
		world->unsubscribeAll(this);
	}

	virtual void tick(class ECS::World* world, float deltaTime) override
	{
		//std::cout << "TestSystem::tick" << std::endl;
	}
};


int main()
{
    ECS::World* world = ECS::World::createWorld();

    ECS::EntitySystem* testSystem = world->registerSystem(new TestSystem());
    ECS::EntitySystem* gamepadSystem = world->registerSystem(new GamepadSystem());

    ECS::Entity* ent = world->create();
	auto gamepadState = ent->assign<GamepadStateComponent>();

    std::cout << "Application Start" << std::endl
		<< "==========================" << std::endl;



    for (;;) 
    {
      
        world->tick(10.f);

        std::cout << std::endl << std::endl << std::endl << std::endl << "main::tick:" << std::endl;
        std::cout << "    t " << gamepadState->throttle << std::endl;
        std::cout << "    y " << gamepadState->yaw << std::endl;
        std::cout << "    p " << gamepadState->pitch << std::endl;
        std::cout << "    r " << gamepadState->roll << std::endl;
        std::cout << "    a " << gamepadState->bA << std::endl;
        std::cout << "    b " << gamepadState->bB << std::endl;
        std::cout << "    x " << gamepadState->bX << std::endl;
        std::cout << "    y " << gamepadState->bY << std::endl;
        std::cout.flush();
        Sleep(10);
    }

    return 0;
}
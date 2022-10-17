#include "console-kb-sys.h"
#include "app-state-comp.h"

#include <conio.h>
#include <stdio.h>

ConsoleKbSystem::~ConsoleKbSystem()
{
}

void ConsoleKbSystem::configure(class ECS::World *world)
{
}

void ConsoleKbSystem::unconfigure(class ECS::World *world)
{
}

void ConsoleKbSystem::tick(class ECS::World *world, float deltaTime)
{
    static AppMode prevAppMode = AppMode::CLOSEST_BATTLE;

    if (_kbhit())
    {
        auto c = _getch();

        if (c == ' ')
        {

            world->each<ApplicationStateComponentSP>(
                [&](ECS::Entity *ent, ECS::ComponentHandle<ApplicationStateComponentSP> aStateH)
                {
                    ApplicationStateComponentSP aState = aStateH.get();
                    if (aState->mode == AppMode::PASSIVE)
                    {
                        aState->mode = prevAppMode;
                    }
                    else
                    {
                        prevAppMode = aState->mode;
                        aState->mode = AppMode::PASSIVE;
                    }
                });
        }
    }
}

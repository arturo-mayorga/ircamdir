#include "console-kb-sys.h"
#include "../components/app-state-comp.h"

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

        AppMode newAppMode = AppMode::MODE_COUNT;

        if (c == ' ')
        {
            world->each<ApplicationStateComponentSP>(
                [&](ECS::Entity *ent, ECS::ComponentHandle<ApplicationStateComponentSP> aStateH)
                {
                    ApplicationStateComponentSP aState = aStateH.get();
                    if (aState->mode == AppMode::PASSIVE)
                    {
                        newAppMode = prevAppMode;
                    }
                    else
                    {
                        prevAppMode = aState->mode;
                        newAppMode = AppMode::PASSIVE;
                    }
                });
        }
        else if (c == 't')
        {
            newAppMode = AppMode::TV_POINT_FILL;
        }
        else if (c == 'b')
        {
            newAppMode = AppMode::CLOSEST_BATTLE;
        }

        if (newAppMode != AppMode::MODE_COUNT)
        {
            world->each<ApplicationStateComponentSP>(
                [&](ECS::Entity *ent, ECS::ComponentHandle<ApplicationStateComponentSP> aStateH)
                {
                    ApplicationStateComponentSP aState = aStateH.get();
                    prevAppMode = aState->mode;
                    aState->mode = newAppMode;
                });
        }
    }
}

#include "gamepad-sys.h"
#include <Windows.h>
#include <XInput.h>

#include <iostream>

#define BMASK_A 4096
#define BMASK_B 8192
#define BMASK_X 16384
#define BMASK_Y 32768

#define AXIS_MAX 32767.0f
#define AXIS_MIN -32768.0f


GamepadSystem::~GamepadSystem() {}

void GamepadSystem::configure(class ECS::World* world)
{
}

void GamepadSystem::unconfigure(class ECS::World* world)
{
    world->unsubscribeAll(this);
}

void GamepadSystem::tick(class ECS::World* world, float deltaTime)
{
    DWORD dwResult;    
    for (DWORD i=0; i< XUSER_MAX_COUNT; i++ )
    {
        XINPUT_STATE state;
        ZeroMemory( &state, sizeof(XINPUT_STATE) );
        
        dwResult = XInputGetState( i, &state );

        if( dwResult == ERROR_SUCCESS )
        {
            // std::cout << std::endl << std::endl << std::endl << std::endl << "GamepadSystem::tick:" << std::endl;

            world->each<GamepadStateComponent>([&](ECS::Entity* ent, ECS::ComponentHandle<GamepadStateComponent> gamepadStateComponent) -> void {
			    gamepadStateComponent->throttle = (state.Gamepad.sThumbLY - AXIS_MIN) / (AXIS_MAX - AXIS_MIN);
                gamepadStateComponent->yaw = state.Gamepad.sThumbLX / AXIS_MAX;
                gamepadStateComponent->pitch = state.Gamepad.sThumbRY / AXIS_MAX;
                gamepadStateComponent->roll = state.Gamepad.sThumbRX / AXIS_MAX;

                gamepadStateComponent->bA = (state.Gamepad.wButtons & BMASK_A) > 0;
                gamepadStateComponent->bB = (state.Gamepad.wButtons & BMASK_B) > 0;
                gamepadStateComponent->bX = (state.Gamepad.wButtons & BMASK_X) > 0;
                gamepadStateComponent->bY = (state.Gamepad.wButtons & BMASK_Y) > 0;

                // std::cout << "    t " << gamepadStateComponent->throttle << std::endl;
                // std::cout << "    y " << gamepadStateComponent->yaw << std::endl;
                // std::cout << "    p " << gamepadStateComponent->pitch << std::endl;
                // std::cout << "    r " << gamepadStateComponent->roll << std::endl;
                // std::cout << "    a " << gamepadStateComponent->bA << std::endl;
                // std::cout << "    b " << gamepadStateComponent->bB << std::endl;
                // std::cout << "    x " << gamepadStateComponent->bX << std::endl;
                // std::cout << "    y " << gamepadStateComponent->bY << std::endl;
		    });

            return;
        }
    }

    // std::cout << "GamepadSystem::tick --" << std::endl;
}

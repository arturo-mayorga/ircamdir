#ifndef gamepad_sys_h_
#define gamepad_sys_h_

#include "ecs.h"

struct GamepadStateComponent
{
	ECS_DECLARE_TYPE;

	GamepadStateComponent() : 
        throttle(0), yaw(0), pitch(0), roll(0),
        bA(0), bB(0), bX(0),  bY(0) {}

	float throttle;
	float yaw;
    float pitch;
    float roll;

    bool bA;
    bool bB;
    bool bX;
    bool bY;
};
ECS_DEFINE_TYPE(GamepadStateComponent);

class GamepadSystem : public ECS::EntitySystem
{
public:
	virtual ~GamepadSystem();

	virtual void configure(class ECS::World* world) override;

	virtual void unconfigure(class ECS::World* world) override;

	virtual void tick(class ECS::World* world, float deltaTime) override;
};

#endif
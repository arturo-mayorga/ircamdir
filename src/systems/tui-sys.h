#ifndef tui_sys_h_
#define tui_sys_h_

#include "../ecs.h"
#include "ftxui/component/loop.hpp"

class TuiSystem : public ECS::EntitySystem
{
private:
    int _appMode = 0;
    ECS::World *_activeWorld = NULL;
    ftxui::Loop *_ftuiLoop = NULL;
    int _isFinished = 0;

public:
    virtual ~TuiSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;

    int isFinished() { return _isFinished; }
};

#endif
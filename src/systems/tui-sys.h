#ifndef tui_sys_h_
#define tui_sys_h_

#include <vector>
#include "../ecs.h"
#include "ftxui/component/loop.hpp"

struct TvDriverTableEntry
{
    std::string name;
    float targetBarVal;
    std::string targetStrVal;
    float actualBarVal;
    std::string actualStrVal;
    int isCarSelected;
    int isCarStatic;
};
typedef std::shared_ptr<TvDriverTableEntry> TvDriverTableEntrySP;

struct DisplayableModel
{
    std::vector<TvDriverTableEntrySP> tvDriverTableEntries;
    std::string currentFrameInfo;
    std::string sessionNameStr;

    int appMode;
    std::vector<std::string> appModeEntries;
};

class TuiSystem : public ECS::EntitySystem
{
private:
    ftxui::Loop *_ftuiLoop = NULL;
    int _isFinished = 0;

    DisplayableModel _dispModel;

public:
    virtual ~TuiSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;

    int isFinished() { return _isFinished; }
};

#endif
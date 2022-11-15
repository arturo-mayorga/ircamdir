#ifndef tui_sys_h_
#define tui_sys_h_

#include <vector>
#include <set>
#include "../ecs.h"
#include "ftxui/component/loop.hpp"
#include "../txt-view/car-event-log-table.h"
#include "../txt-view/tv-driver-table.h"

struct DisplayableModel
{
    std::vector<TvDriverTableEntrySP> tvDriverTableEntries;
    std::vector<CarEventTableEntrySP> overtakeLog;
    std::vector<CarEventTableEntrySP> detectedIncidentLog;

    std::string currentFrameInfo;
    std::string sessionNameStr;

    int appMode;
    std::vector<std::string> appModeEntries;

    ECS::World *world;
};

class TuiSystem : public ECS::EntitySystem
{
private:
    ftxui::Loop *_ftuiLoop = NULL;
    int _isFinished = 0;

    DisplayableModel _dispModel;

    std::set<int> _seenOvertakes;
    std::set<int> _seenDetectedIncidents;

    void _onEventLogClicked(int frameNum, int carIdx);

public:
    virtual ~TuiSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;

    int isFinished() { return _isFinished; }
};

#endif
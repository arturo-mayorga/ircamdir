#ifndef car_comp_h_
#define car_comp_h_

#include "ecs.h"
#include <string>
#include <memory>

enum CarType
{
    PACE,
    RACE,
    OTHER
};

struct StaticCarStateComponent
{
    ECS_DECLARE_TYPE;

    StaticCarStateComponent() : idx(-1), uid(-1), name("") {}

    int idx;
    int uid;
    std::string name;
};
ECS_DEFINE_TYPE(StaticCarStateComponent);
typedef std::shared_ptr<StaticCarStateComponent> StaticCarStateComponentSP;

struct DynamicCarStateComponent
{
    ECS_DECLARE_TYPE;

    DynamicCarStateComponent() : lapDistPct(-1), idx(-1), officialPos(0), isInPits(0) {}

    float lapDistPct;
    int idx;
    int officialPos;
    int isInPits;
};
ECS_DEFINE_TYPE(DynamicCarStateComponent);
typedef std::shared_ptr<DynamicCarStateComponent> DynamicCarStateComponentSP;

#endif
#ifndef car_comp_h_
#define car_comp_h_

#include "../ecs.h"
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

    StaticCarStateComponent() : idx(-1), uid(-1), name(""), number(-1) {}

    int idx;
    int uid;
    std::string name;
    int number;
};
ECS_DEFINE_TYPE(StaticCarStateComponent);
typedef std::shared_ptr<StaticCarStateComponent> StaticCarStateComponentSP;

struct DynamicCarStateComponent
{
    ECS_DECLARE_TYPE;

    DynamicCarStateComponent() : lapDistPct(-1), idx(-1), officialPos(0), isInPits(0) {}

    float lapDistPct;
    float deltaLapDistPct;
    int idx;
    int officialPos;
    int isInPits;
    int currentLap;
};
ECS_DEFINE_TYPE(DynamicCarStateComponent);
typedef std::shared_ptr<DynamicCarStateComponent> DynamicCarStateComponentSP;

struct BroadcastCarInfoComponent
{
    ECS_DECLARE_TYPE;

    BroadcastCarInfoComponent() : idx(-1), scrTime(0), leadTime(0), top3Time(0), top5Time(0), top10Time(0), top20Time(0), tvPoints(0) {}

    int idx;
    float scrTime;
    float leadTime;
    float top3Time;
    float top5Time;
    float top10Time;
    float top20Time;
    float tvPoints;
};
ECS_DEFINE_TYPE(BroadcastCarInfoComponent);
typedef std::shared_ptr<BroadcastCarInfoComponent> BroadcastCarInfoComponentSP;

struct BroadcastCarSummaryComponent
{
    ECS_DECLARE_TYPE;

    BroadcastCarSummaryComponent() : idx(-1), tvPtsPct(0), scrTimePct(0) {}

    int idx;
    float tvPtsPct;
    float scrTimePct;
};
ECS_DEFINE_TYPE(BroadcastCarSummaryComponent);
typedef std::shared_ptr<BroadcastCarSummaryComponent> BroadcastCarSummaryComponentSP;

struct CarEvent
{
    int frameNumber;
    int lapNumber;
    int carIdx;
    int secCarIdx;
};
typedef std::shared_ptr<CarEvent> CarEventSP;

struct OvertakeSummaryComponent
{
    ECS_DECLARE_TYPE;

    std::vector<CarEventSP> events;
};
ECS_DEFINE_TYPE(OvertakeSummaryComponent);
typedef std::shared_ptr<OvertakeSummaryComponent> OvertakeSummaryComponentSP;

struct DetectedIncidentSummaryComponent
{
    ECS_DECLARE_TYPE;

    std::vector<CarEventSP> events;
};
ECS_DEFINE_TYPE(DetectedIncidentSummaryComponent);
typedef std::shared_ptr<DetectedIncidentSummaryComponent> DetectedIncidentSummaryComponentSP;

#endif
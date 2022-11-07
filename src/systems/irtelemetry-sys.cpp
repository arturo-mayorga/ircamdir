#include "irtelemetry-sys.h"
#include "../components/car-comp.h"
#include "../components/cam-ctrl-comp.h"
#include "../components/session-comp.h"

#include "../ecs-util.h"

#include <Windows.h>

#include <iostream>

#define MIN_WIN_VER 0x0501

#ifndef WINVER
#define WINVER MIN_WIN_VER
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT MIN_WIN_VER
#endif

#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

#include <signal.h>
#include <time.h>

#include "../irsdk/irsdk_defines.h"
#include "../irsdk/irsdk_client.h"
#include "../irsdk/yaml_parser.h"

#include <vector>
#include <map>
#include <set>
#include <memory>

// for timeBeginPeriod
#pragma comment(lib, "Winmm")

irsdkCVar g_camCarIdx("CamCarIdx");
irsdkCVar g_camGroup("CamGroupNumber");

irsdkCVar g_sessionNum("SessionNum");

irsdkCVar g_CarIdxLapDistPct("CarIdxLapDistPct");
irsdkCVar g_carIdxClassPosition("CarIdxClassPosition");
irsdkCVar g_isCarInPits("CarIdxTrackSurface");

irsdkCVar g_replayFrameNum("ReplayFrameNum");
irsdkCVar g_replayFrameNumEnd("ReplayFrameNumEnd");

void monitorConnectionStatus()
{
    // keep track of connection status
    bool isConnected = irsdkClient::instance().isConnected();
    static bool wasConnected = !isConnected;
    if (wasConnected != isConnected)
    {
        if (isConnected)
        {
            // printf("Connected to iRacing\n");
        }
        else
        {
            //   printf("Lost connection to iRacing\n");
        }
        wasConnected = isConnected;
    }
}

std::map<int, std::string> getSessionNameMap(const char *yaml)
{
    std::map<int, std::string> ret;

    char valstr[512];
    int valstrlen = 512;
    char str[512];

    const char *tVal = NULL;
    int tValLen = 0;

    int i = 0;

    do
    {
        sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:{%d}SessionName:", i);
        if (parseYaml(yaml, str, &tVal, &tValLen))
        {
            int len = tValLen;
            if (len > 512)
                len = 512;

            // copy what we can, even if buffer too small
            memcpy(valstr, tVal, len);
            valstr[len] = '\0'; // original string has no null termination...

            std::string name(valstr);
            int sessionNum = -1;

            // std::cout << " session data :: " << name << " ";

            if (i == 0)
            {
                sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:");
            }
            else
            {
                sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:{%d}SessionNum:", i - 1);
            }
            if (parseYaml(yaml, str, &tVal, &tValLen))
            {
                int len = tValLen;
                if (len > 512)
                    len = 512;

                // copy what we can, even if buffer too small
                memcpy(valstr, tVal, len);
                valstr[len] = '\0'; // original string has no null termination...
                sessionNum = atoi(valstr);
                // std::cout << sessionNum << "\n";
            }

            ret[sessionNum] = name;

            ++i;
        }
        else
        {
            i = -1;
        }
    } while (i != -1);

    return ret;
}

bool invalidChar(char c)
{
    return !(c >= 0 && c < 128);
}

std::map<int, StaticCarStateComponentSP> getStaticCarStates(const char *yaml)
{
    // TODO: figure out a cheaper way to do this
    std::map<int, StaticCarStateComponentSP> ret;
    char valstr[512];
    int valstrlen = 512;
    char str[512];

    const char *tVal = NULL;
    int tValLen = 0;

    int i = 0;

    do
    {
        sprintf_s(str, 512, "DriverInfo:Drivers:CarIdx:{%d}UserName:", i);
        if (parseYaml(yaml, str, &tVal, &tValLen))
        {
            int len = tValLen;
            if (len > 512)
                len = 512;

            // copy what we can, even if buffer too small
            memcpy(valstr, tVal, len);
            valstr[len] = '\0'; // original string has no null termination...

            StaticCarStateComponentSP cState(new StaticCarStateComponent());
            cState->name = valstr;

            cState->name.erase(remove_if(cState->name.begin(), cState->name.end(), invalidChar), cState->name.end());

            // std::cout << ":: " << valstr << " ";

            if (i == 0)
            {
                sprintf_s(str, 512, "DriverInfo:Drivers:CarIdx:");
            }
            else
            {
                sprintf_s(str, 512, "DriverInfo:Drivers:CarIdx:{%d}CarIdx:", i - 1);
            }
            if (parseYaml(yaml, str, &tVal, &tValLen))
            {
                int len = tValLen;
                if (len > 512)
                    len = 512;

                // copy what we can, even if buffer too small
                memcpy(valstr, tVal, len);
                valstr[len] = '\0'; // original string has no null termination...
                cState->idx = atoi(valstr);
                // std::cout << valstr << "\n";
            }

            sprintf_s(str, 512, "DriverInfo:Drivers:CarIdx:{%d}UserID:", i);
            if (parseYaml(yaml, str, &tVal, &tValLen))
            {
                int len = tValLen;
                if (len > 512)
                    len = 512;

                // copy what we can, even if buffer too small
                memcpy(valstr, tVal, len);
                valstr[len] = '\0'; // original string has no null termination...
                cState->uid = atoi(valstr);
            }

            sprintf_s(str, 512, "DriverInfo:Drivers:CarIdx:{%d}CarNumberRaw:", i);
            if (parseYaml(yaml, str, &tVal, &tValLen))
            {
                int len = tValLen;
                if (len > 512)
                    len = 512;

                // copy what we can, even if buffer too small
                memcpy(valstr, tVal, len);
                valstr[len] = '\0'; // original string has no null termination...
                cState->number = atoi(valstr);
            }

            ret[cState->idx] = cState;

            ++i;
        }
        else
        {
            i = -1;
        }
    } while (i != -1);

    return ret;
}

IrTelemetrySystem::~IrTelemetrySystem()
{
}

void IrTelemetrySystem::configure(class ECS::World *world)
{
    world->subscribe<OnCameraChangeRequest>(this);
}

void IrTelemetrySystem::unconfigure(class ECS::World *world)
{
    world->unsubscribeAll(this);
}

void IrTelemetrySystem::receive(ECS::World *world, const OnCameraChangeRequest &event)
{

    int requestedCarIdx = event.targetCarIdx;
    int requestedCarNum = 0;
    std::string requestedCarName;

    int camGroup = g_camGroup.getInt();

    world->each<StaticCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<StaticCarStateComponentSP> cStateH)
        {
            StaticCarStateComponentSP cState = cStateH.get();

            int i = cState->idx;

            if (cState->idx == requestedCarIdx)
            {
                requestedCarNum = cState->number;
            }
        });

    if (SpecialCarNum::LEADER == requestedCarIdx)
    {
        requestedCarNum = irsdk_csMode::irsdk_csFocusAtLeader;
    }
    else if (SpecialCarNum::EXITING == requestedCarIdx)
    {
        requestedCarNum = irsdk_csMode::irsdk_csFocusAtExiting;
    }
    else if (SpecialCarNum::INCIDENT == requestedCarIdx)
    {
        requestedCarNum = irsdk_csMode::irsdk_csFocusAtIncident;
    }

    irsdk_broadcastMsg(irsdk_BroadcastCamSwitchNum, requestedCarNum, camGroup, 0);
}

void IrTelemetrySystem::tick(class ECS::World *world, float deltaTime)
{
    static int first = 1;
    static int lastCam = -1;
    static float tSinceCamChange = 0;
    static float tSinceIrData = 0;
    static std::map<int, std::string> sessionNameMap;

    tSinceCamChange += deltaTime;
    tSinceIrData += deltaTime;

    // wait up to 16 ms for start of session or new data
    if (irsdkClient::instance().waitForData(16))
    {
        // and grab the data
        if (first)
        {
            first = 0;

            auto cStates = getStaticCarStates(irsdk_getSessionInfoStr());
            sessionNameMap = getSessionNameMap(irsdk_getSessionInfoStr());

            // std::cout << "there were " << cStates.size() << " cars" << std::endl;

            std::set<int> existingCarIdxs;

            std::vector<ECS::Entity *> entsToDestroy;

            world->each<StaticCarStateComponentSP>(
                [&](ECS::Entity *ent, ECS::ComponentHandle<StaticCarStateComponentSP> cStateH)
                {
                    StaticCarStateComponentSP cState = cStateH.get();

                    if (cStates.count(cState->idx))
                    {
                        existingCarIdxs.insert(cState->idx);
                        cState->idx = cStates[cState->idx]->idx;
                        cState->number = cStates[cState->idx]->number;
                    }
                    else
                    {
                        entsToDestroy.push_back(ent);
                    }
                });

            for (auto pair : cStates)
            {
                if (existingCarIdxs.count(pair.first))
                {
                    // std::cout << "found car for " << pair.second->name << std::endl;
                }
                else
                {
                    // std::cout << "creating car for " << pair.second->name << " " << pair.second->idx << std::endl;

                    ECS::Entity *ent = world->create();
                    auto staticCarState = ent->assign<StaticCarStateComponentSP>(new StaticCarStateComponent());
                    auto dynamicCarState = ent->assign<DynamicCarStateComponentSP>(new DynamicCarStateComponent());
                    auto broadcastCarState = ent->assign<BroadcastCarInfoComponentSP>(new BroadcastCarInfoComponent());
                    auto broadcastCarSummary = ent->assign<BroadcastCarSummaryComponentSP>(new BroadcastCarSummaryComponent());

                    staticCarState.get()->idx = pair.second->idx;
                    staticCarState.get()->name = pair.second->name;
                    staticCarState.get()->uid = pair.second->uid;
                    staticCarState.get()->number = pair.second->number;

                    dynamicCarState.get()->idx = pair.second->idx;
                    broadcastCarState.get()->idx = pair.second->idx;
                    broadcastCarSummary.get()->idx = pair.second->idx;
                }
            }
        }

        world->each<DynamicCarStateComponentSP>(
            [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
            {
                DynamicCarStateComponentSP cState = cStateH.get();

                int i = cState->idx;

                if (i >= 0)
                {
                    // std::cout << "getting dynamic info for " << i << std::endl;
                    float oldLapPct = cState->lapDistPct;
                    cState->lapDistPct = g_CarIdxLapDistPct.getFloat(i);
                    cState->officialPos = (int)g_carIdxClassPosition.getFloat(i);
                    cState->isInPits = ((int)g_isCarInPits.getFloat(i)) == irsdk_TrkLoc::irsdk_InPitStall;

                    float deltaLapDistPct = cState->lapDistPct - oldLapPct;
                    if (deltaLapDistPct < 0)
                    {
                        deltaLapDistPct += 1;
                    }

                    deltaLapDistPct /= tSinceIrData;
                    cState->deltaLapDistPct = deltaLapDistPct;
                }
                else
                {
                    // std::cout << "got a bad idx\n";
                }
            });

        SessionComponentSP sessionComp = ECSUtil::getFirstCmp<SessionComponentSP>(world);
        sessionComp->num = g_sessionNum.getInt();
        sessionComp->name = sessionNameMap[sessionComp->num];

        tSinceIrData = 0;
    }

    CameraActualsComponentSP cameraActualsCmp = ECSUtil::getFirstCmp<CameraActualsComponentSP>(world);
    cameraActualsCmp->timeSinceLastChange = tSinceCamChange;
    cameraActualsCmp->replayFrameNum = g_replayFrameNum.getInt();
    cameraActualsCmp->replayFrameNumEnd = g_replayFrameNumEnd.getInt();
    cameraActualsCmp->currentCarIdx = g_camCarIdx.getInt();
    if (cameraActualsCmp->currentCarIdx != lastCam)
    {
        tSinceCamChange = 0;
        lastCam = cameraActualsCmp->currentCarIdx;
    }

    // your normal process loop would go here
    monitorConnectionStatus();
}

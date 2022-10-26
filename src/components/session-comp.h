#ifndef session_comp_h_
#define session_comp_h_

#include "../ecs.h"
#include <string>

struct SessionComponent
{
    ECS_DECLARE_TYPE;

    SessionComponent() : num(0), name("") {}

    int num;
    std::string name;
};
ECS_DEFINE_TYPE(SessionComponent);
typedef std::shared_ptr<SessionComponent> SessionComponentSP;

#endif
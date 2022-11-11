#ifndef tv_driver_table_h_
#define tv_driver_table_h_

#include "ftxui/component/component.hpp"

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

ftxui::Component TvDriverTable(std::vector<TvDriverTableEntrySP> &cars);

#endif
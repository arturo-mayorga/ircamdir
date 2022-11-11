#ifndef car_event_log_table_h_
#define car_event_log_table_h_

#include "ftxui/component/component.hpp"

struct CarEventTableEntry
{
    std::string driverName;
    std::string eventNote;
    int frameNumber;
    int carIdx;
};
typedef std::shared_ptr<CarEventTableEntry> CarEventTableEntrySP;

ftxui::Component CarEventLogTable(
    std::vector<CarEventTableEntrySP> &overtakeLog,
    std::function<void(int frameNum, int carIdx)> on_click);

#endif
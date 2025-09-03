#ifndef SCHEDULE_HPP
#define SCHEDULE_HPP

#include <set>
#include <string>
#include <timeslot.hpp>
#include <vector>

class Schedule {
 public:
  Schedule() = default;

  bool IsAvailable(TimeSlot time) const;
  void MarkAvailable(TimeSlot time);
  void MarkBusy(TimeSlot time);

 private:
  std::set<TimeSlot> times_;
};

#endif
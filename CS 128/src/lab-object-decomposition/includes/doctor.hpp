#ifndef DOCTOR_HPP
#define DOCTOR_HPP

#include <schedule.hpp>
#include <set>
#include <string>
#include <timeslot.hpp>
#include <vector>

class Doctor {
 public:
  Doctor() = default;

  bool IsAvailable(TimeSlot time) const;
  void AddAppointment(TimeSlot time);
  void RemoveAppointment(TimeSlot time);

 private:
  Schedule schedule_;
};

#endif
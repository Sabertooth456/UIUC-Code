#ifndef HOSPITAL_HPP
#define HOSPITAL_HPP

#include <appointment.hpp>
#include <doctor.hpp>
#include <set>
#include <string>
#include <vector>

class Hospital {
 public:
  Hospital() = default;
  void AddDoctor(Doctor& d);
  Doctor* GetAvailableDoctor(TimeSlot time);
  void AddAppointment(TimeSlot time, std::string patient);
  void RemoveAppointment(TimeSlot time, std::string patient);

 private:
  std::vector<Doctor*> doctors_;
  std::vector<Appointment> appointments_;
};

#endif
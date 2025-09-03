#include "student.hpp"

#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "course.hpp"

std::ostream& operator<<(std::ostream& os, const Student& s) {
  os << "Name: " << s.GetName() << std::endl;
  os << "UIN: " << s.GetUIN() << std::endl;
  os << "Net Id: " << s.GetNetId() << std::endl;
  os << "Major: " << s.GetMajor() << std::endl;

  std::vector<Course> courses = s.GetCourses();
  int accumulated_credits = 0;
  for (Course course : courses) {
    accumulated_credits += course.credits;
  }
  os << "Credits: " << accumulated_credits << std::endl;

  os << "Courses: ";
  if (courses.size() > 0) {
    for (unsigned int i = 0; i < courses.size() - 1; i++) {
      os << courses[i].name << ", ";
    }
    os << courses.back().name;
  }
  os << std::endl;

  os << "Graduated: ";
  if (s.HasGraduated()) {
    os << "True";
  } else {
    os << "False";
  }

  // "Name: Ike South\nUIN: 000000000\nNet Id: ike\nMajor: "
  //         "Undeclared\nCredits: 0\nCourses: \nGraduated: False"

  return os;
}

// Name: [name_]
// UIN: [uin_]
// Net Id: [net_id_]
// Major: [major_]
// Credits: [credits_]
// Courses: [courses_[0]], [courses_[1]], ..., [courses.back()]
// Graduated: [graduated_]

Student::Student(std::string name, std::string uin, std::string net_id) {
  name_ = name;
  uin_ = uin;
  net_id_ = net_id;
  major_ = "Undeclared";
  graduated_ = false;
}

Student::Student(std::string name, std::string uin, std::string net_id,
                 std::string major, std::vector<Course> courses,
                 bool graduated) {
  name_ = name;
  uin_ = uin;
  net_id_ = net_id;
  major_ = major;
  courses_ = courses;
  graduated_ = graduated;
}

std::string Student::GetName() const { return name_; }
std::string Student::GetUIN() const { return uin_; }
std::string Student::GetNetId() const { return net_id_; }
std::string Student::GetMajor() const { return major_; }
bool Student::HasGraduated() const { return graduated_; }
const std::vector<Course>& Student::GetCourses() const { return courses_; }
void Student::SetName(std::string name) { name_ = name; }
void Student::SetMajor(std::string major) { major_ = major; }
bool Student::AddCourse(Course c) {
  for (Course course : courses_) {
    if (course.name == c.name) {
      return false;
    }
  }
  courses_.push_back(c);
  return true;
}
void Student::Graduate() {
  int accumulated_credits = 0;
  for (Course course : courses_) {
    accumulated_credits += course.credits;
  }
  if (accumulated_credits >= min_credits_grad_) {
    graduated_ = true;
  }
}
#include "bathroom.hpp"

Bathroom::Bathroom() : Room("bathroom") {}
// std::string Bathroom::GetName() const { return this->GetName(); }
bool Bathroom::CanAddPerson() const { return people_.empty(); }
// void Bathroom::AddPerson(std::string name) {}
// void RemovePerson(std::string name);
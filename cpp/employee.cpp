#include"employee.h"
#include "InvalidDataException.h"
Employee::Employee(std::string name, std::string lastName, std::string middleName, std::string phoneNumber, std::string shift, std::string position) :Person(name, lastName, middleName, phoneNumber) {
	if (shift.empty()) 
		throw InvalidDataException("Employee shift cannot be empty");
	if (position.empty()) 
		throw InvalidDataException("Employee position cannot be empty");
	
	this->shift = shift;
	this->position = position;
}
std::string Employee::getType() const{
	return "Employee";
}
std::string Employee::getPosition()const {
	return this->position;
}
void Employee::assignShift(std::string newShift) {
	if (newShift.empty())
		throw InvalidDataException("Employee shift cannot be empty");
	this->shift = newShift;
}
std::string Employee::getShift()const {
	return this->shift;
}
void Employee::printInfo(std::ostream& os)const {
	Person::printInfo(os);
	os << "\nThe shift: " << this->getShift() << "\nActual work position: " << this->getPosition();
}
Employee::~Employee(){
	std::cout << this->getFullName() << ": object employee successfuly deleted.\n";
}
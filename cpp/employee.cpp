#include"employee.h"
Employee::Employee(std::string name, std::string lastName, std::string middleName, std::string phoneNumber, std::string shift, std::string position) :Person(name, lastName, middleName, phoneNumber) {
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
	this->shift = newShift;
}
std::string Employee::getShift()const {
	return this->shift;
}
void Employee::printInfo()const {
	Person::printInfo();
	std::cout << "\nThe shift: " << this->getShift() << "\nActual work position: " << this->getPosition();
}
std::ostream& operator<<(std::ostream& os, Employee& object) {
	object.printInfo();
	return os;
}
Employee::~Employee(){
	std::cout << this->getFullName() << ": object employee successfuly deleted.\n";
}
#include "person.h"
Person::Person(std::string name, std::string lastName, std::string middleName, std::string phoneNumber) {
	this->name = name;
	this->lastName = lastName;
	this->middleName = middleName;
	this->phoneNumber = phoneNumber;
}
std::string Person::getFullName() const{
	if (middleName.empty())
		return this->lastName + " " + this->name;
	return this->lastName + " " + this->name + " " + this->middleName;
}
std::string Person::getPhoneNumber() const{
	return this->phoneNumber;
}
void Person::printInfo() const{
	std::cout << "\nName: " << getFullName() << "\nPhone number: " << getPhoneNumber();
}
Person::~Person() {
	std::cout << "Class Person successfully have been deleted.\n";
}
#include "person.h"
#include "InvalidPhoneNumberException.h"
#include "InvalidDataException.h"
namespace {
	bool isValidPhoneNumber(const std::string& number) {
		int length = static_cast<int>(number.size());
		if (length != 13)
			return false;
		if (number.substr(0, 4) != "+375") 
			return false;
		for (int i = 4; i < length; i++)
			if (number[i] < '0' || number[i]>'9')
				return false;
		return true;
	}
}
Person::Person(std::string name, std::string lastName, std::string middleName, std::string phoneNumber) {
	this->name = name;
	this->lastName = lastName;
	this->middleName = middleName;
	this->phoneNumber = phoneNumber;

	if (!isValidPhoneNumber(phoneNumber))
		throw InvalidPhoneNumberException(phoneNumber);
	if (name.empty() || lastName.empty()) 
		throw InvalidDataException("Name and last name cannot be empty");
}
std::string Person::getFullName() const{
	if (middleName.empty())
		return this->lastName + " " + this->name;
	return this->lastName + " " + this->name + " " + this->middleName;
}
std::string Person::getPhoneNumber() const{
	return this->phoneNumber;
}
void Person::printInfo(std::ostream& os) const{
	os << "\nName: " << getFullName() << "\nPhone number: " << getPhoneNumber();
}
Person::~Person() {
	std::cout << "Class Person successfully have been deleted.\n";
}
std::ostream& operator<<(std::ostream& os, const Person& object) {
	object.printInfo(os);
	return os;
}
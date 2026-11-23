#include "client.h"
#include "InvalidDataException.h"
Client::Client(std::string name, std::string lastName, std::string middleName, std::string phoneNumber, int bonus) :Person(name, lastName, middleName, phoneNumber) {
	if(bonus<0)
		throw InvalidDataException("Bonus points cannot be negative");
	this->bonus = bonus;
}
int Client::getBonus() const{
	return this->bonus;
}
void Client::setBonus(int bonus) {
	if (bonus < 0)
		throw InvalidDataException("Bonus points cannot be negative");
	this->bonus = bonus;
}
std::string Client::getType() const{
	return "Client";
}
void Client::printInfo(std::ostream& os) const{
	Person::printInfo(os);
	os << "\nBonus point: " << getBonus() << "\n";
}
Client::~Client() {
	std::cout << this->getFullName() << ": object client successfuly deleted.\n";
}
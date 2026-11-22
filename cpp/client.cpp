#include "client.h"
Client::Client(std::string name, std::string lastName, std::string middleName, std::string phoneNumber, int bonus) :Person(name, lastName, middleName, phoneNumber) {
	this->bonus = bonus;
}
int Client::getBonus() const{
	return this->bonus;
}
void Client::setBonus(int bonus) {
	this->bonus = bonus;
}
std::string Client::getType() const{
	return "Client";
}
void Client::printInfo() const{
	Person::printInfo();
	std::cout << "\nBonus point: " << getType() << "\n";
}
std::ostream& operator<<(std::ostream& os, Client& object) {
	object.printInfo();
	return os;
}
Client::~Client() {
	std::cout << this->getFullName() << ": object client successfuly deleted.\n";
}
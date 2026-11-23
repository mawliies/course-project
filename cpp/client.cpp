#include "client.h"
Client::Client(std::string name, std::string lastName, std::string middleName, std::string phoneNumber, int bonus) :Person(name, lastName, middleName, phoneNumber) {
	this->bonus = bonus;
}
int Client::getBonus() const{
	return this->bonus;
}
void Client::setBonus(int bonus) {
	//>=0
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
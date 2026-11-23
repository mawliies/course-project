#include"drink.h"
#include "InvalidAmountException.h"
Drink::Drink(std::string title, float price, std::string category, const Recipe& recipe, int volumeML, bool isCold) :MenuObject(title, price, category, recipe) {
	if (volumeML <= 0)
		throw InvalidAmountException("Drink volume must be positive");
	this->volumeML = volumeML;
	this->isCold = isCold;
}
int Drink::getVolume()const {
	return this->volumeML;
}
bool Drink::getIsCold()const {
	return this->isCold;
}
std::string Drink::getType()const {
	return "Drink";
}
void Drink::printInfo()const {
	MenuObject::printInfo();
	std::cout << "\nThe volume: " << this->getVolume() << " ml";
	std::cout << "\nTemprature: ";
	if (this->getIsCold())
		std::cout << "cold";
	else
		std::cout << "hot";
}
Drink::~Drink() {
	std::cout << this->getTitle() << ": drink have been deleted.\n";
}
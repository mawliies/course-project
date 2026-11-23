#include"dish.h"
#include "InvalidAmountException.h"
Dish::Dish(std::string title, float price, std::string category, const Recipe& recipe, int time, int weight) :MenuObject(title, price, category, recipe) {
	if (time < 0)
		throw InvalidAmountException("Cooking time cannot be negative");
	if (weight <= 0)
		throw InvalidAmountException("Dish weight must be positive");
	this->cookingTime = time;
	this->weight = weight;
}
int Dish::getTime()const {
	return this->cookingTime;
}
int Dish::getWeight()const {
	return this->weight;
}
std::string Dish::getType()const {
	return "Dish";
}
void Dish::printInfo()const {
	MenuObject::printInfo();
	std::cout << "\nThe weight: " << this->getWeight() << " g";
	std::cout << "\nCooking time: " << this->getTime() << " min\n";
}
Dish::~Dish() {
	std::cout << this->getTitle() << " :the dish have been deleted.\n";
}
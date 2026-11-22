#include "ingredient.h"
#include <iostream>
Ingredient::Ingredient(std::string name, const NutritionValue& nutritionPer100, MeasureUnit unit) : name(name), nutritionPer100(nutritionPer100), unit(unit) {
	this->name = name;
	this->nutritionPer100 = nutritionPer100;
	this->unit = unit;
}
std::string Ingredient::getName() const{
	return this->name;
}
MeasureUnit Ingredient::getUnit() const {
	return this->unit;
}
NutritionValue Ingredient::getNutrition()const {
	return this->nutritionPer100;
}
void Ingredient::printInfo() const{
	std::cout << *this;
}
std::ostream& operator<<(std::ostream& os, const Ingredient& object) {
	os << "\nIngredient: " << object.getName();
	os << "\nNutrition Value: " << object.getNutrition();
	return os;
}
Ingredient::~Ingredient() {
	std::cout << this->getName() << " have been deleted from ingredient's list.\n";
}
#include "ingredient.h"
#include "InvalidDataException.h"
#include <iostream>
Ingredient::Ingredient(std::string name, const NutritionValue& nutritionPer100, MeasureUnit unit) : name(name), nutritionPer100(nutritionPer100), unit(unit), id(0) {
	if (name.empty())
		throw InvalidDataException("Ingredient name cannot be empty");
	if (unit != MeasureUnit::gram && unit != MeasureUnit::milliliter)
		throw InvalidDataException("Invalid ingredient measure unit");
}
Ingredient::Ingredient(int id, std::string name, const NutritionValue& nutritionPer100, MeasureUnit unit) : name(name), nutritionPer100(nutritionPer100), unit(unit), id(id) {
	if (id <= 0)
		throw InvalidDataException("Invalid ingredient ID");
	if (name.empty())
		throw InvalidDataException("Ingredient name cannot be empty");
	if (unit != MeasureUnit::gram &&
		unit != MeasureUnit::milliliter)
		throw InvalidDataException("Invalid ingredient measure unit");
}
int Ingredient::getId() const {
	return id;
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
Ingredient::~Ingredient() {}
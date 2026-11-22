#pragma once
#include <string>
#include "nutritionValue.h"
#include "measureUnit.h"
class Ingredient {
private:
	std::string name;
	NutritionValue nutritionPer100;
	MeasureUnit unit;
public:
	Ingredient(std::string name, const NutritionValue& nutritionPer100, MeasureUnit unit);
	std::string getName()const;
	MeasureUnit getUnit()const;
	NutritionValue getNutrition()const;
	void printInfo()const;
	friend std::ostream& operator<<(std::ostream& os, const Ingredient& object);
	~Ingredient();
};
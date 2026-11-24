#pragma once
#include <string>
#include "nutritionValue.h"
#include "measureUnit.h"

class IngredientCatalog;

class Ingredient {
private:
	std::string name;
	NutritionValue nutritionPer100;
	MeasureUnit unit;
	int id;
	friend class IngredientCatalog;
public:
	Ingredient(std::string name, const NutritionValue& nutritionPer100, MeasureUnit unit);
	Ingredient(int id, std::string name, const NutritionValue& nutritionPer100, MeasureUnit unit);
	int getId() const;
	std::string getName()const;
	MeasureUnit getUnit()const;
	NutritionValue getNutrition()const;
	void printInfo()const;
	friend std::ostream& operator<<(std::ostream& os, const Ingredient& object);
	~Ingredient();
};
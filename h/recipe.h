#pragma once
#include <vector>
#include "ingredientCatalog.h"
#include "nutritionValue.h"
struct ResipeIngredient {
	std::string name;
	float amount;
};

class Recipe {
private:
	std::vector<ResipeIngredient> ingredientsRecipe;
	int portions;
public:
	Recipe(int portions);
	void addIngredient(float amount,std::string name,const IngredientCatalog& catalog);
	void removeIngredient(std::string name);
	NutritionValue calculateNutrition(const IngredientCatalog& catalog)const;
	int getPortions()const;
	void setPortions(int portions);
	friend std::ostream& operator<<(std::ostream& os, const Recipe& recipe);
	~Recipe() = default;
 };
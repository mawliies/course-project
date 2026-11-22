#pragma once
#include "Collection.h"
#include "ingredient.h"
#include<vector>
class IngredientCatalog {
private:
	std::vector<Ingredient> ingredients;
public:
	IngredientCatalog();
	IngredientCatalog& operator+=(const Ingredient& ingredient);
	IngredientCatalog& operator-=(const Ingredient& ingredient);
	void addIngredient();
	void removeIngredient(std::string name);
	const Ingredient* findByName(std::string name)const;
	void printAll() const;
	~IngredientCatalog();
};
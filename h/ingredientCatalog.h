#pragma once
#include "Collection.h"
#include "ingredient.h"
#include<vector>
class IngredientCatalog {
private:
	std::vector<Ingredient> ingredients;
	int nextId = 1;
public:
	IngredientCatalog();
	IngredientCatalog& operator+=(const Ingredient& ingredient);
	IngredientCatalog& operator-=(const Ingredient& ingredient);
	const std::vector<Ingredient>& getAll() const;
	int getNextId()const;
	void setNextId(int nextId);
	void addIngredient();
	void removeIngredient(std::string name);
	const Ingredient* findByName(std::string name)const;
	const Ingredient* findById(int id) const;
	void printAll() const;
	~IngredientCatalog();
};
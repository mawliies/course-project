#include "recipe.h"
Recipe::Recipe(int portions) {
	//!=0<0
	this->portions = portions;
}
void Recipe::addIngredient(float amount, std::string name, const IngredientCatalog& catalog) {
	const Ingredient* check = catalog.findByName(name);
	if (check == nullptr) {
		//nullptr
	}
	for (int i = 0; i < ingredientsRecipe.size(); i++)
		if (ingredientsRecipe[i].name == name) {
			//дублирование
		}
	ResipeIngredient newIngredient;
	newIngredient.name = name;
	newIngredient.amount = amount;
	ingredientsRecipe.push_back(newIngredient);
}
void Recipe::removeIngredient(std::string name) {
	for (int i = 0; i < ingredientsRecipe.size(); i++)
		if (ingredientsRecipe[i].name == name) {
			ingredientsRecipe.erase(ingredientsRecipe.begin() + i);
			return;
		}
	//nullptr	
}
std::ostream& operator<<(std::ostream& os, const Recipe& recipe) {
	os << "\nPortions: " << recipe.portions;
	os << "\nIngredients:\n";

	for (int i = 0; i < recipe.ingredientsRecipe.size(); i++) {
		os << recipe.ingredientsRecipe[i].name << " - ";
		os << recipe.ingredientsRecipe[i].amount << "\n";
	}

	return os;
}
NutritionValue Recipe::calculateNutrition(const IngredientCatalog& catalog) const{
	float cal = 0;
	float protein = 0;
	float fats = 0;
	float carbs = 0;
	for (int i = 0; i < ingredientsRecipe.size(); i++) {
		const Ingredient* ingredient = catalog.findByName(ingredientsRecipe[i].name);
		if (ingredient == nullptr) {
			//nullptr
		}
		NutritionValue nutrition = ingredient->getNutrition();
		float coefficient = ingredientsRecipe[i].amount / 100.0f;
		cal += nutrition.getCalories() * coefficient;
		protein += nutrition.getProteins() * coefficient;
		fats += nutrition.getFats() * coefficient;
		carbs += nutrition.getCarbs() * coefficient;
	}
	cal /= portions;
	protein /= portions;
	fats /= portions;
	carbs /= portions;

	return NutritionValue(cal, protein, fats, carbs);
}
int Recipe::getPortions()const {
	return this->portions;
}
void Recipe::setPortions(int portions) {
	//!0<0
	this->portions = portions;
}
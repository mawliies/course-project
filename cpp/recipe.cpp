#include "recipe.h"
#include "AllExceptions.h"
Recipe::Recipe(int portions) {
	setPortions(portions);
}
void Recipe::addIngredient(float amount, std::string name, const IngredientCatalog& catalog) {
	if (name.empty())
		throw InvalidDataException("Ingredient name cannot be empty");
	if (amount <= 0)
		throw InvalidAmountException("Ingredient amount must be positive");

	const Ingredient* check = catalog.findByName(name);

	if (check == nullptr) 
		throw NotFoundException(name);
	for (int i = 0; i < ingredientsRecipe.size(); i++)
		if (ingredientsRecipe[i].ingredientId == check->getId()) 
			throw DuplicateDataException(name);
	RecipeIngredient newIngredient;
	newIngredient.name = check->getName();
	newIngredient.amount = amount;
	newIngredient.ingredientId = check->getId();
	ingredientsRecipe.push_back(newIngredient);
}
void Recipe::removeIngredient(std::string name) {
	for (int i = 0; i < ingredientsRecipe.size(); i++)
		if (ingredientsRecipe[i].name == name) {
			ingredientsRecipe.erase(ingredientsRecipe.begin() + i);
			return;
		}
	throw NotFoundException(name);
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
const std::vector<RecipeIngredient>& Recipe::getIngredients() const {
	return ingredientsRecipe;
}
NutritionValue Recipe::calculateNutrition(const IngredientCatalog& catalog) const{
	float cal = 0;
	float protein = 0;
	float fats = 0;
	float carbs = 0;
	for (int i = 0; i < ingredientsRecipe.size(); i++) {
		const Ingredient* ingredient = catalog.findById(ingredientsRecipe[i].ingredientId);

		if (ingredient == nullptr) 
			throw NotFoundException(ingredientsRecipe[i].name);

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
	if (portions <= 0)
		throw InvalidAmountException("Number of portions must be positive");
	this->portions = portions;
}
#include"ingredientCatalog.h"
#include"AllExceptions.h"
#include<iostream>
IngredientCatalog::IngredientCatalog() {}
IngredientCatalog& IngredientCatalog::operator+=(const Ingredient& ingredient) {
	if (findByName(ingredient.getName()) != nullptr) 
		throw DuplicateDataException(ingredient.getName());
	this->ingredients.push_back(ingredient);
	return *this;
}
IngredientCatalog& IngredientCatalog::operator-=(const Ingredient& ingredient) {
	std::string name = ingredient.getName();
	for (int i = 0; i < (ingredients.size()); i++)
		if (ingredients[i].getName() == name) {
			ingredients.erase(ingredients.begin() + i);
			break;
		}
	return *this;
	throw NotFoundException(name);
}
const Ingredient* IngredientCatalog::findByName(std::string name) const{
	for (int i = 0; i < ingredients.size(); i++) 
		if (ingredients[i].getName() == name) 
			return &ingredients[i];
	return nullptr;
}
void IngredientCatalog::addIngredient() {
	std::string name;
	NutritionValue nutrition(0, 0, 0, 0);
	int unitnum;

	std::cout << "\nEnter the name of the ingredient: ";
	if (!(std::cin >> name)) 
		throw InvalidDataException("Incorrect ingredient name input");
	std::cout << "\nEnter the nutrition value (example: calories/protein/fats/carbs): ";
	if (!(std::cin >> nutrition))
		throw InvalidDataException("Incorrect nutrition value input");
	std::cout << "\nEnter the measure unit\n 1.gram\n2.mililiter\n";
	if (!(std::cin >> unitnum)) 
		throw InvalidDataException("Incorrect measure unit input");
	if (unitnum != 1 && unitnum != 2) 
		throw InvalidDataException("Unknown measure unit");

	MeasureUnit unit = static_cast<MeasureUnit>(unitnum - 1);
	Ingredient newIngredient(name,nutrition,unit);
	*this += newIngredient;
}
void IngredientCatalog::removeIngredient(std::string name) {
	const Ingredient* check = findByName(name);
	if (check == nullptr)
		throw NotFoundException(name);
	*this -= *check;
}
void IngredientCatalog::printAll() const {
	for (int i = 0; i < ingredients.size(); i++)
		ingredients[i].printInfo();
}
IngredientCatalog::~IngredientCatalog() {
	ingredients.clear();
	std::cout << "Ingredient catalog have been successfully deleted.\n";
}
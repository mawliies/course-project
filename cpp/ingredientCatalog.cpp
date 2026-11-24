#include"ingredientCatalog.h"
#include"AllExceptions.h"
#include<iostream>
IngredientCatalog::IngredientCatalog() {}
IngredientCatalog& IngredientCatalog::operator+=(const Ingredient& ingredient) {
	if (findByName(ingredient.getName()) != nullptr) 
		throw DuplicateDataException(ingredient.getName());
	Ingredient newIngredient = ingredient;

	if (newIngredient.id == 0) 
		newIngredient.id = nextId++;
	else {
		if (newIngredient.id < 0)
			throw InvalidDataException("Invalid ingredient ID");
		if (findById(newIngredient.id) != nullptr)
			throw DuplicateDataException("Duplicate ingredient ID: " + std::to_string(newIngredient.id));
		if (newIngredient.id >= nextId)
			nextId = newIngredient.id + 1;
	}
	ingredients.push_back(newIngredient);
	return *this;
}
const std::vector<Ingredient>& IngredientCatalog::getAll() const {
	return this->ingredients;
}
int IngredientCatalog::getNextId()const {
	return this->nextId;
}
void IngredientCatalog::setNextId(int nextId) {
	if (nextId <= 0)
		throw InvalidDataException("Invalid next ingredient ID");

	for (int i = 0; i < ingredients.size(); i++) {
		if (ingredients[i].getId() >= nextId)
			throw InvalidDataException("Invalid next ingredient ID");
	}
	this->nextId = nextId;
}
IngredientCatalog& IngredientCatalog::operator-=(const Ingredient& ingredient) {
	std::string name = ingredient.getName();
	for (int i = 0; i < (ingredients.size()); i++)
		if (ingredients[i].getName() == name) {
			ingredients.erase(ingredients.begin() + i);
			return *this;
		}
	throw NotFoundException(name);
}
const Ingredient* IngredientCatalog::findByName(std::string name) const{
	for (int i = 0; i < ingredients.size(); i++) 
		if (ingredients[i].getName() == name) 
			return &ingredients[i];
	return nullptr;
}
const Ingredient* IngredientCatalog::findById(int id) const {
	for (int i = 0; i < ingredients.size(); i++) 
		if (ingredients[i].getId() == id)
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
	std::cout << "Ingredient catalog have been successfully deleted.\n";
}
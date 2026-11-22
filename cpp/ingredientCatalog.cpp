#include"ingredientCatalog.h"
#include<iostream>
IngredientCatalog::IngredientCatalog() {}
IngredientCatalog& IngredientCatalog::operator+=(const Ingredient& ingredient) {
	//проверка на дублирование и nullptr
	this->ingredients.push_back(ingredient);
	return *this;
}
IngredientCatalog& IngredientCatalog::operator-=(const Ingredient& ingredient) {
	//nullptr
	std::string name = ingredient.getName();
	for(int i =0;i<ingredients.size();i++)
		if (ingredients[i].getName() == name) {
			ingredients.erase(ingredients.begin() + i);
			break;
		}
	return *this;
}
const Ingredient* IngredientCatalog::findByName(std::string name) const{
	//nullptr
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
	std::cin >> name;
	const Ingredient* check = findByName(name);
	if (check!=nullptr){
		//дублирование
	}
	std::cout << "\nEnter the nutrition value (example: calories/protein/fats/carbs): ";
	std::cin >> nutrition;
	std::cout << "\nEnter the measure unit\n 1.gram\n2.mililiter\n";
	std::cin >> unitnum;
	MeasureUnit unit = static_cast<MeasureUnit>(unitnum - 1);
	Ingredient newIngredient(name,nutrition,unit);
	*this += newIngredient;
}
void IngredientCatalog::removeIngredient(std::string name) {
	const Ingredient* check = findByName(name);
	if (check != nullptr)
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
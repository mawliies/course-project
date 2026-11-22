#include"menuObject.h"
MenuObject::MenuObject(std::string title, float price, std::string category, const Recipe& recipe) : title(title), price(price), category(category), recipe(recipe) {}
std::string MenuObject::getTitle()const {
	return this->title;
}
float MenuObject::getPrice()const {
	return this->price;
}
std::string MenuObject::getCategory()const {
	return this->category;
}
const Recipe& MenuObject::getRecipe()const {
	return this->recipe;
}
void MenuObject::printInfo()const {
	std::cout << "\n---" << this->getTitle() << "---\n";
	std::cout << "\nCategory: " << this->category;
	std::cout << "\n---The recipe---" << this->getRecipe();
	std::cout << "\nPrice: " << this->getPrice() << " $";
}
MenuObject::~MenuObject() {}
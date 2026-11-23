#include "menu.h"
#include"AllExceptions.h"
Menu::Menu() {}
void Menu::addToMenu(MenuObject* object) {
	if (object == nullptr)
		throw InvalidDataException("Cannot add null menu object");
	for (int i = 0; i < static_cast<int>(items.size()); i++) 
		if (items[i] != nullptr && (items[i] == object || items[i]->getTitle() == object->getTitle())) 
			throw DuplicateDataException(object->getTitle());
	items.push_back(object);
}
const MenuObject* Menu::findInMenu(const std::string& name)const {
	for (int i = 0; i < items.size(); i++)
		if (items[i]->getTitle() == name)
			return items[i];
	return nullptr;
}
void Menu::removeFromMenu(std::string name) {
	const MenuObject* object = findInMenu(name);
	if (object == nullptr)
		throw NotFoundException(name);
	for(int i=0;i<items.size();i++)
		if (items[i] == object) {
			delete items[i];
			items.erase(items.begin() + i);
			return;
		}
}
void Menu::printMenu()const {
	if (items.empty()) {
		std::cout << "Menu is empty.\n";
		return;
	}
	for (int i = 0; i < items.size(); i++) 
		if (items[i] != nullptr)
			items[i]->printInfo();
}
Menu::~Menu() {
	for (int i = 0; i < items.size(); i++)
		delete items[i];
	items.clear();
}
bool isLeapYear(int year) {
	return (year % 400 == 0) ||
		(year % 4 == 0 && year % 100 != 0);
}

int daysInMonth(int month, int year) {
	switch (month) {
	case 2:
		return isLeapYear(year) ? 29 : 28;
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;
	default:
		return 0;
	}
}
bool isValidDate(int dd, int mm, int yyyy) {
	if (yyyy <= 0 || mm < 1 || mm > 12 || dd < 1)
		return false;

	return dd <= daysInMonth(mm, yyyy);
}
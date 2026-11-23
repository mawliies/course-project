#include "menu.h"
Menu::Menu() {}
void Menu::addToMenu(MenuObject* object) {
	//nullptr
	items.push_back(object);
}
const MenuObject* Menu::findInMenu(const std::string& name)const {
	for (int i = 0; i < items.size(); i++)
		if (items[i]->getTitle() == name)
			return items[i];
	//проверка если не найдено + nullptr
}
void Menu::removeFromMenu(std::string name) {
	const MenuObject* object = findInMenu(name);
	//nullptr
	for(int i=0;i<items.size();i++)
		if (items[i] == object) {
			delete items[i];
			items.erase(items.begin() + i);
			return;
		}
}
void Menu::printMenu()const {
	//empty menu
	for (int i = 0; i < items.size(); i++)
		items[i]->printInfo();
}
Menu::~Menu() {
	for (int i = 0; i < items.size(); i++)
		delete items[i];
	items.clear();
}
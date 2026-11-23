#include "mainMenu.h"
MainMenu::MainMenu(int dd, int mm, int yyyy) {
	this->dd = dd;
	this->mm = mm;
	this->yyyy = yyyy;
}
int MainMenu::getDay()const {
	return this->dd;
}
int MainMenu::getMonth()const {
	return this->mm;
}
int MainMenu::getYear()const {
	return this->yyyy;
}
void MainMenu::setDate(int dd, int mm, int yyyy) {
	this->dd = dd;
	this->mm = mm;
	this->yyyy = yyyy;
}
void MainMenu::printMenu()const {
	std::cout << "\n---MAIN MENU---\n";
	Menu::printMenu();
	std::cout << "\n\n\nDate of updating: " << this->getDay() << "." << this->getMonth() << "." << this->getYear();
}
bool MainMenu::isAvailable()const {
	return true;
}
MainMenu::~MainMenu() {}
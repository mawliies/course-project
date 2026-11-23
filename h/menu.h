#pragma once
#include<string>
#include<vector>
#include<iostream>
#include"menuObject.h"
class Menu {
protected:
	std::vector<MenuObject*>items;
public:
	//хранение будет в файлах, это мы сделаем попозже, вместе с исключениями
	Menu();
	void addToMenu(MenuObject* object);
	void removeFromMenu(std::string name);
	const MenuObject* findInMenu(const std::string& name)const;
	virtual void printMenu()const;
	virtual bool isAvailable()const = 0;
	virtual ~Menu();
};

bool isLeapYear(int year);
int daysInMonth(int month, int year);
bool isValidDate(int dd, int mm, int yyyy);
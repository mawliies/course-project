#pragma once
#include "menu.h"
class MainMenu :public Menu {
private:
	int dd, mm, yyyy;
public:
	MainMenu();
	MainMenu(int dd, int mm, int yyyy);
	int getDay()const;
	int getMonth()const;
	int getYear()const;
	void setDate(int dd, int mm, int yyyy);
	void printMenu()const override;
	bool isAvailable()const override;
	~MainMenu();
};
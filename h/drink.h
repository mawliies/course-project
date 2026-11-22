#pragma once
#include "menuObject.h"
class Drink :public MenuObject {
private:
	int volumeML;
	bool isCold;
public:
	Drink(std::string title, float price, std::string category, const Recipe& recipe, int volumeML, bool isCold);
	int getVolume()const;
	bool getIsCold()const;
	std::string getType()const override;
	void printInfo()const override;
	~Drink();
};
#pragma once
#include "menuObject.h"
class Dish :public MenuObject {
private:
	int cookingTime;
	int weight;
public:
	Dish(std::string title, float price, std::string category, const Recipe& recipe, int time, int weight);
	int getTime()const;
	int getWeight()const;
	std::string getType()const override;
	void printInfo()const override;
	~Dish();
};
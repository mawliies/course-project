#pragma once
#include <string>
#include"recipe.h"
class MenuObject {
protected:
	std::string title;
	float price;
	std::string category;
	Recipe recipe;
public:
	MenuObject(std::string title, float price, std::string category, const Recipe& recipe);
	std::string getTitle()const;
	float getPrice()const;
	std::string getCategory()const;
	const Recipe& getRecipe()const;
	virtual std::string getType()const = 0;
	virtual void printInfo()const;
	virtual~MenuObject();
};
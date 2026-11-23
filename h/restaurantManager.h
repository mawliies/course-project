#pragma once
#include"collection.h"
#include"ingredientCatalog.h"
#include"mainMenu.h"
#include"seasonalMenu.h"
#include"client.h"
#include"employee.h"
#include"order.h"
#include<vector>
#include<iostream>
#include<string>
class RestaurantManager {
private:
	IngredientCatalog catalog;
	MainMenu mainMenu;
	SeasonalMenu seasonalMenu;
	Collection<Person*> people;
	std::vector<Order> orders;
public:
	RestaurantManager();
	~RestaurantManager();
};
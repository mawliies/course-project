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

	void addIngredient(const Ingredient& ingredient);
	void removeIngredient(const std::string& name);
	const Ingredient* findIngredient(const std::string& name) const;
	void printIngredients() const;

	void addToMainMenu(MenuObject* object);
	void addToSeasonalMenu(MenuObject* object);
	void removeFromMainMenu(const std::string& name);
	void removeFromSeasonalMenu(const std::string& name);
	const MenuObject* findInMainMenu(const std::string& name) const;
	const MenuObject* findInSeasonalMenu(const std::string& name) const;
	void printMainMenu() const;
	void printSeasonalMenu() const;
	bool isSeasonalMenuAvailable() const;

	void setMainMenuUpdateDate(int dd, int mm, int yyyy);
	void setSeasonalMenuPeriod(int ddBegin, int mmBegin, int yyyyBegin, int ddEnd, int mmEnd, int yyyyEnd);

	void addPerson(Person* person);
	void removePerson(const std::string& phoneNumber);
	Person* findPerson(const std::string& phoneNumber) const;
	void printPeople() const;

	std::size_t createOrder();
	void addItemToOrder(std::size_t orderIndex, const std::string& itemName, int amount);
	void removeItemFromOrder(std::size_t orderIndex, const std::string& itemName);
	float calculateOrderTotal(std::size_t orderIndex) const;
	void changeOrderStatus(std::size_t orderIndex,OrderStatus status);
};
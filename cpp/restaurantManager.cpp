#include"restaurantManager.h"
#include "AllExceptions.h"
RestaurantManager::RestaurantManager(){}

void RestaurantManager::addPerson(Person* person) {
	if (person == nullptr)
		throw InvalidDataException("Cannot add null person");
	if (findPerson(person->getPhoneNumber()) != nullptr)
		throw DuplicateDataException(person->getPhoneNumber());

	people.add(person);
}
Person* RestaurantManager::findPerson(const std::string& phoneNumber) {
	for (int i = 0; i < people.getSize(); i++)
		if (people.get(i)->getPhoneNumber() == phoneNumber)
			return people.get(i);
	return nullptr;
}
void RestaurantManager::removePerson(const std::string& phoneNumber) {
	Person* current = findPerson(phoneNumber);
	if (current == nullptr)
		throw NotFoundException(phoneNumber);
	for (int i = 0; i < people.getSize(); i++)
		if (people.get(i) == current) {
			people.removeAt(i);
			delete current;
			return;
		}
}
void RestaurantManager::printPeople(std::ostream& os) const {
	for (int i = 0; i < people.getSize(); i++)
		if (people.get(i) != nullptr)
			os << *people.get(i) << "\n";
}





void RestaurantManager::addIngredient(const Ingredient& ingredient) {
	catalog += ingredient;
}
void RestaurantManager::removeIngredient(const std::string& name) {
	catalog.removeIngredient(name);
}
const Ingredient* RestaurantManager::findIngredient(const std::string& name) const {
	return catalog.findByName(name);
}
void RestaurantManager::printIngredients() const {
	catalog.printAll();
}





void RestaurantManager::addToMainMenu(MenuObject* object) {
	if (object != nullptr && seasonalMenu.findInMenu(object->getTitle()) == object)
		throw DuplicateDataException("Object already belongs to seasonal menu: " + object->getTitle());

	mainMenu.addToMenu(object);
}
void RestaurantManager::addToSeasonalMenu(MenuObject* object) {
	if (object != nullptr && seasonalMenu.findInMenu(object->getTitle()) == object)
		throw DuplicateDataException("Object already belongs to main menu: " + object->getTitle());
	seasonalMenu.addToMenu(object);
}
void RestaurantManager::removeFromMainMenu(const std::string& name) {
	mainMenu.removeFromMenu(name);
}
void RestaurantManager::removeFromSeasonalMenu(const std::string& name) {
	seasonalMenu.removeFromMenu(name);
}
const MenuObject* RestaurantManager::findInMainMenu(const std::string& name) const {
	return mainMenu.findInMenu(name);
}
const MenuObject* RestaurantManager::findInSeasonalMenu(const std::string& name) const {
	return seasonalMenu.findInMenu(name);
}
void RestaurantManager::printMainMenu() const {
	mainMenu.printMenu();
}
void RestaurantManager::printSeasonalMenu() const {
	seasonalMenu.printMenu();
}
bool RestaurantManager::isSeasonalMenuAvailable() const {
	return seasonalMenu.isAvailable();
}
void RestaurantManager::setMainMenuUpdateDate(int dd, int mm, int yyyy) {
	mainMenu.setDate(dd, mm, yyyy);
}
void RestaurantManager::setSeasonalMenuPeriod(int ddBegin, int mmBegin, int yyyyBegin, int ddEnd, int mmEnd, int yyyyEnd) {
	if (!isValidDate(ddBegin, mmBegin, yyyyBegin))
		throw InvalidDateException("Invalid beginning date");
	if (!isValidDate(ddEnd, mmEnd, yyyyEnd))
		throw InvalidDateException("Invalid ending date");

	int beginDate = yyyyBegin * 10000 + mmBegin * 100 + ddBegin;
	int endDate = yyyyEnd * 10000 + mmEnd * 100 + ddEnd;

	if (beginDate > endDate)
		throw InvalidDateRangeException(ddBegin, mmBegin, yyyyBegin, ddEnd, mmEnd, yyyyEnd);
	if (seasonalMenu.getDayBegin() == 0 ||seasonalMenu.getDayEnd() == 0) {
		seasonalMenu.setBeginDate(ddBegin, mmBegin, yyyyBegin);
		seasonalMenu.setEndDate(ddEnd, mmEnd, yyyyEnd);
	}
	else {
		int oldEndDate = seasonalMenu.getYearEnd() * 10000 + seasonalMenu.getMonthEnd() * 100 + seasonalMenu.getDayEnd();
		if (beginDate > oldEndDate) {
			seasonalMenu.setEndDate(ddEnd, mmEnd, yyyyEnd);
			seasonalMenu.setBeginDate(ddBegin, mmBegin, yyyyBegin);
		}
		else {
			seasonalMenu.setBeginDate(ddBegin, mmBegin, yyyyBegin);
			seasonalMenu.setEndDate(ddEnd, mmEnd, yyyyEnd);
		}
	}
}





void RestaurantManager::addOrder(const Order& order) {
	Person* person = findPerson(order.getClientPhoneNumber());

	if (person == nullptr)
		throw NotFoundException("Client: " + order.getClientPhoneNumber());
	if (dynamic_cast<Client*>(person) == nullptr)
		throw InvalidOperationException("Only clients can have orders");

	orders.push_back(order);
}
void RestaurantManager::removeOrder(int index) {
	if (index < 0 || index >= static_cast<int>(orders.size()))
		throw OutOfRangeException(index, static_cast<int>(orders.size()));
	orders.erase(orders.begin() + index);
}
Order& RestaurantManager::findOrder(int index) {
	if (index < 0 || index >= static_cast<int>(orders.size()))
		throw OutOfRangeException(index, static_cast<int>(orders.size()));

	return orders[index];
}
const Order& RestaurantManager::findOrder(int index) const {
	if (index < 0 || index >= static_cast<int>(orders.size()))
		throw OutOfRangeException(index, static_cast<int>(orders.size()));

	return orders[index];
}
void RestaurantManager::addItemToOrder(int orderIndex, const MenuObject& object, int amount) {
	findOrder(orderIndex).addItem(object, amount);
}
void RestaurantManager::removeItemFromOrder(int orderIndex, const std::string& name) {
	findOrder(orderIndex).removeItem(name);
}
float RestaurantManager::calculateOrderTotal(int orderIndex) const {
	return findOrder(orderIndex).calculateTotal();
}
void RestaurantManager::setOrderStatus(int orderIndex, OrderStatus status) {
	findOrder(orderIndex).setStatus(status);
}
OrderStatus RestaurantManager::getOrderStatus(int orderIndex) const {
	return findOrder(orderIndex).getStatus();
}
int RestaurantManager::getOrdersCount() const {
	return orders.size();
}





RestaurantManager::~RestaurantManager() {
	for (int i = 0; i < people.getSize(); i++)
		delete people.get(i);
}
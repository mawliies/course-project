#include"restaurantManager.h"
RestaurantManager::RestaurantManager(){}

void RestaurantManager::addPerson(Person* person) {
	//nullptr
	people.add(person);
}
Person* RestaurantManager::findPerson(const std::string& phoneNumber) const {
	//nullptr
	for (int i = 0; i < people.getSize(); i++)
		if (people.get(i)->getPhoneNumber() == phoneNumber)
			return people.get(i);
}
void RestaurantManager::removePerson(const std::string& phoneNumber) {
	//nullptr
	Person* current = findPerson(phoneNumber);
	for (int i = 0; i < people.getSize(); i++)
		if (people.get(i) == current)
			people.removeAt(i);
}
void RestaurantManager::printPeople() const {
	for (int i = 0; i < people.getSize(); i++)
		people.get(i)->printInfo();//сюда подавалось os. как тошда быть
	//nullptr
}

RestaurantManager::~RestaurantManager() {}
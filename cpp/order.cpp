#include "order.h"
Order::Order(const std::string& clientPhoneNumber, OrderStatus status) : clientPhoneNumber(clientPhoneNumber), status(status) {}
void Order::addItem(const MenuObject& object, int amount) {
	if (amount <= 0) {
		//
	}
	OrderItem newItem;
	newItem.name = object.getTitle();
	newItem.amount = amount;
	newItem.priceAtOrder = object.getPrice();
	items.push_back(newItem);
}
void Order::removeItem(std::string name) {
	for(int i=0;i<items.size();i++)
		if (items[i].name == name) {
			items.erase(items.begin() + i);
			return;
		}
	//если не было наййдено
}
float Order::calculateTotal()const {
	float total = 0;
	for (int i = 0; i < items.size(); i++)
		total += items[i].amount * items[i].priceAtOrder;
	return total;
}
std::string Order::getClientPhoneNumber() const {
	return this->clientPhoneNumber;
}
OrderStatus Order::getStatus()const {
	return this->status;
}
void Order::setStatus(OrderStatus status) {
	this->status=status;
}
Order::~Order() {}
#include "order.h"
#include"AllExceptions.h"
Order::Order(const std::string& clientPhoneNumber, OrderStatus status) : clientPhoneNumber(clientPhoneNumber), status(status) {}
void Order::addItem(const MenuObject& object, int amount) {
	if (amount <= 0)
		throw InvalidAmountException("Order item amount must be positive");

	OrderItem newItem;
	newItem.name = object.getTitle();
	newItem.amount = amount;
	newItem.priceAtOrder = object.getPrice();
	items.push_back(newItem);
}
const std::vector<OrderItem>& Order::getItems()const {
	return this->items;
}
void Order::removeItem(std::string name) {
	for(int i=0;i<items.size();i++)
		if (items[i].name == name) {
			items.erase(items.begin() + i);
			return;
		}
	throw NotFoundException(name);
}
void Order::restore(const OrderItem& item) {
	if (item.name.empty())
		throw InvalidDataException("Order item name cannot be empty");
	if (item.amount <= 0)
		throw InvalidAmountException("Order item amount must be positive");
	if (item.priceAtOrder <= 0)
		throw InvalidPriceException(std::to_string(item.priceAtOrder));

	items.push_back(item);
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
	switch (status) {
	case OrderStatus::Draft:
	case OrderStatus::New:
	case OrderStatus::Ready:
	case OrderStatus::Served:
	case OrderStatus::Cancelled:
		break;
	default:
		throw InvalidDataException("Unknown order status");
	}
	return this->status;
}
void Order::setStatus(OrderStatus status) {
	this->status=status;
}
Order::~Order() {}
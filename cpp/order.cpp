#include "order.h"
#include"AllExceptions.h"
#include <cmath>
Order::Order(const std::string& clientPhoneNumber, OrderStatus status, int guestCount) : clientPhoneNumber(clientPhoneNumber), status(status), guestCount(guestCount), tableNumber(0), loyaltyDiscount(0.0f), redeemedBonusPoints(0) {

	if (clientPhoneNumber.empty())
		throw InvalidDataException("Client phone number cannot be empty");
	if (guestCount <= 0)
		throw InvalidAmountException("Guest count must be positive");
}
void Order::addItem(const MenuObject& object, int amount) {
	if (amount <= 0)
		throw InvalidAmountException("Order item amount must be positive");
	OrderItem newItem;
	newItem.name = object.getTitle();
	newItem.amount = amount;
	newItem.priceAtOrder = object.getPrice();
	items.push_back(newItem);
}
int Order::getGuestCount() const {
	return guestCount;
}
int Order::getTableNumber() const {
	return tableNumber;
}
bool Order::hasAssignedTable() const {
	return tableNumber != 0;
}
void Order::setTableNumber(int tableNumber) {
	if (tableNumber < 0)
		throw InvalidDataException("Table number cannot be negative");
	this->tableNumber = tableNumber;
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
	for (int i = 0; i < static_cast<int>(items.size()); i++)
		total += items[i].amount * items[i].priceAtOrder;
	return total;
}
float Order::calculatePayableTotal() const {
	const float payable = calculateTotal() - loyaltyDiscount;
	return payable > 0.0f ? payable : 0.0f;
}
float Order::getLoyaltyDiscount() const {
	return loyaltyDiscount;
}
int Order::getRedeemedBonusPoints() const {
	return redeemedBonusPoints;
}
void Order::applyLoyaltyRedemption(int points, float discountAmount) {
	if (points < 0 || discountAmount < 0.0f)
		throw InvalidAmountException("Loyalty redemption cannot be negative");
	const float expectedDiscount = static_cast<float>(points) / static_cast<float>(LoyaltyPolicy::pointsPerBynDiscount);
	if (std::fabs(expectedDiscount - discountAmount) > 0.0051f)
		throw InvalidDataException("Loyalty redemption must use 100 points per 1 BYN");
	if (discountAmount > calculateTotal() + 0.0001f)
		throw InvalidOperationException("Loyalty discount cannot exceed the order total");
	loyaltyDiscount = discountAmount;
	redeemedBonusPoints = points;
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
	case OrderStatus::Preparing:
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
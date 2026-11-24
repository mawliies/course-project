#pragma once
#include <string>
#include"collection.h"
#include"menuObject.h"
#include<vector>
struct OrderItem {
	std::string name;
	int amount;
	float priceAtOrder;
};

enum class OrderStatus {
	Draft,
	New,
	Ready,
	Served,
	Cancelled
};

class Order {
private:
	std::vector<OrderItem>items;
	std::string clientPhoneNumber;
	OrderStatus status;
public:
	Order(const std::string& clientPhoneNumber, OrderStatus status = OrderStatus::Draft);
	const std::vector<OrderItem>& getItems()const;
	void addItem(const MenuObject& object, int amount);
	void removeItem(std::string name);
	void restore(const OrderItem& item);
	float calculateTotal()const;
	std::string getClientPhoneNumber() const;
	OrderStatus getStatus()const;
	void setStatus(OrderStatus status);
	~Order();
};
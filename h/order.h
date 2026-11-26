#pragma once
#include <string>
#include"collection.h"
#include"menuObject.h"
#include<vector>

class RestaurantManager;

namespace LoyaltyPolicy {
    inline constexpr int pointsPerBynDiscount = 100;
    inline constexpr int pointsEarnedPerByn = 1;
}

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
	Cancelled,
	Preparing // appended to preserve numeric values in existing database files
};

class Order {
private:
    std::vector<OrderItem> items;
    std::string clientPhoneNumber;
    OrderStatus status;
    int guestCount;
    int tableNumber;
    float loyaltyDiscount;
    int redeemedBonusPoints;
    void setTableNumber(int tableNumber);
    void setStatus(OrderStatus status);
    friend class RestaurantManager;

public:
    Order(const std::string& clientPhoneNumber, OrderStatus status = OrderStatus::Draft, int guestCount = 1);
    const std::vector<OrderItem>& getItems() const;
    void addItem(const MenuObject& object, int amount);
    void removeItem(std::string name);
    void restore(const OrderItem& item);
    float calculateTotal() const;
    float calculatePayableTotal() const;
    float getLoyaltyDiscount() const;
    int getRedeemedBonusPoints() const;
    void applyLoyaltyRedemption(int points, float discountAmount);
    std::string getClientPhoneNumber() const;
    int getGuestCount() const;
    int getTableNumber() const;
    bool hasAssignedTable() const;
    OrderStatus getStatus() const;
    ~Order();
};
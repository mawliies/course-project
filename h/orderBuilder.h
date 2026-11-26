#pragma once
#include "nutritionRequest.h"
#include "restaurantManager.h"
#include "nutritionValue.h"
#include "order.h"
#include <vector>
#include <string>
#include <cstddef>

struct OrderSelection {
    const MenuObject* object;
    int amount;
};

class OrderRecommendation {
private:
    std::vector<OrderSelection> items;
    NutritionValue totalNutrition;
    float totalPrice;
    float score;
    std::string explanation;

public:
    OrderRecommendation(const std::vector<OrderSelection>& items, const NutritionValue& totalNutrition, float totalPrice, float score, const std::string& explanation);
    const std::vector<OrderSelection>& getItems() const;
    const NutritionValue& getTotalNutrition() const;
    float getTotalPrice() const;
    float getScore() const;
    const std::string& getExplanation() const;
};

class OrderBuilder {
private:
    struct Candidate {
        const MenuObject* object;
        NutritionValue nutrition;
        float price;
    };
public:
    bool containsExcludedIngredient(const MenuObject* object, const NutritionRequest& request) const;
    bool satisfiesLimits(const NutritionValue& nutrition, const NutritionRequest& request) const;
    float calculateScore(const NutritionValue& nutrition, const NutritionRequest& request) const;
    std::string buildExplanation() const;
    void searchCombinations(const std::vector<Candidate>& candidates, const NutritionRequest& request, std::size_t minItems, std::size_t maxItems, std::size_t startIndex, std::vector<OrderSelection>& selected, float calories, float proteins, float fats, float carbs, float price, std::vector<OrderRecommendation>& results) const;
    std::vector<OrderRecommendation> buildRecommendations(const RestaurantManager& manager, const NutritionRequest& request, std::size_t minItems = 2, std::size_t maxItems = 3, std::size_t maxResults = 5) const;
    Order createOrder(const OrderRecommendation& recommendation, const std::string& clientPhoneNumber, int guestCount = 1) const;
    int confirmRecommendation(const std::vector<OrderRecommendation>& recommendations, std::size_t selectedIndex, const std::string& clientPhoneNumber, RestaurantManager& manager, int guestCount = 1) const;
};


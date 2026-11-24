#pragma once
#include "nutritionRequest.h"
#include "restaurantManager.h"
#include <map>
#include <cstddef>
#include <string>
#include <vector>

class Recommendation {
private:
    const MenuObject* menuObject;
    NutritionValue nutrition;
    float score;
    std::string explanation;
public:
    Recommendation(const MenuObject* menuObject, const NutritionValue& nutrition, float score, const std::string& explanation);
    const MenuObject* getMenuObject() const;
    const NutritionValue& getNutrition() const;
    float getScore() const;
    const std::string& getExplanation() const;
};

class NutritionAdvisor {
private:
    bool isSuitable(const Recommendation& recommendation, const NutritionRequest& request) const;
    float calculateScore(const NutritionValue& nutrition, const NutritionRequest& request) const;
    std::string buildExplanation(const NutritionValue& nutrition, const NutritionRequest& request) const;
public:
    std::vector<Recommendation> recommend(const RestaurantManager& manager, const NutritionRequest& request) const;
    std::map<std::string, std::vector<Recommendation>> groupByCategory(const std::vector<Recommendation>& recommendations) const;
    const Recommendation* findByTitle(const std::vector<Recommendation>& recommendations,const std::string& title) const;
    const Recommendation* findLowestCalories(const std::vector<Recommendation>& recommendations) const;
    const Recommendation* findHighestProtein(const std::vector<Recommendation>& recommendations) const;
    std::size_t countAtLeastScore(const std::vector<Recommendation>& recommendations,float minimumScore) const;
    float calculateAverageScore(const std::vector<Recommendation>& recommendations) const;
};
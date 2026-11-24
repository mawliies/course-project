#pragma once
#include <set>
#include <string>

class NutritionRequest {
private:
    float maxCalories;
    float minProteins;
    float maxFats;
    float maxCarbs;
    std::set<std::string> excludedIngredients;

public:
    NutritionRequest();
    std::string normalizeName(const std::string& name)const;
    void setMaxCalories(float value);
    void setMinProteins(float value);
    void setMaxFats(float value);
    void setMaxCarbs(float value);
    float getMaxCalories() const;
    float getMinProteins() const;
    float getMaxFats() const;
    float getMaxCarbs() const;
    void addExcludedIngredient(const std::string& name);
    bool removeExcludedIngredient(const std::string& name);
    bool isIngredientExcluded(const std::string& name) const;
    const std::set<std::string>& getExcludedIngredients() const;
    bool hasNutritionLimits() const;
    bool hasCriteria() const;
};

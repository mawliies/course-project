#pragma once
#include <vector>
#include "ingredientCatalog.h"
#include "nutritionValue.h"
struct RecipeIngredient {
    std::string name;
    float amount;
    int ingredientId;
};

class Recipe {
private:
    std::vector<RecipeIngredient> ingredientsRecipe;
    int portions;
public:
    Recipe(int portions);
    const std::vector<RecipeIngredient>& getIngredients() const;
    void addIngredient(float amount, std::string name, const IngredientCatalog& catalog);
    void removeIngredient(std::string name);
    NutritionValue calculateNutrition(const IngredientCatalog& catalog) const;
    int getPortions() const;
    void setPortions(int portions);
    friend std::ostream& operator<<(std::ostream& os, const Recipe& recipe);
    ~Recipe() = default;
};
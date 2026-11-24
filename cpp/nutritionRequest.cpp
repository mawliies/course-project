#include "nutritionRequest.h"
#include "AllExceptions.h"
#include <algorithm>
#include <cctype>
#include <cmath>

NutritionRequest::NutritionRequest() : maxCalories(0.0f), minProteins(0.0f), maxFats(0.0f), maxCarbs(0.0f) {}

std::string NutritionRequest::normalizeName(const std::string& name) const{
    std::string result = name;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char character) {
            return static_cast<char>(std::tolower(character));
        });
    return result;
}
void NutritionRequest::setMaxCalories(float value) {
    if (!std::isfinite(value) || value < 0.0f)
        throw InvalidDataException("Maximum calories must be a finite non-negative value");
    maxCalories = value;
}
void NutritionRequest::setMinProteins(float value) {
    if (!std::isfinite(value) || value < 0.0f)
        throw InvalidDataException("Minimum proteins must be a finite non-negative value");
    minProteins = value;
}
void NutritionRequest::setMaxFats(float value) {
    if (!std::isfinite(value) || value < 0.0f)
        throw InvalidDataException("Maximum fats must be a finite non-negative value");
    maxFats = value;
}
void NutritionRequest::setMaxCarbs(float value) {
    if (!std::isfinite(value) || value < 0.0f)
        throw InvalidDataException("Maximum carbohydrates must be a finite non-negative value");
    maxCarbs = value;
}
float NutritionRequest::getMaxCalories() const {
    return maxCalories;
}
float NutritionRequest::getMinProteins() const {
    return minProteins;
}
float NutritionRequest::getMaxFats() const {
    return maxFats;
}
float NutritionRequest::getMaxCarbs() const {
    return maxCarbs;
}
void NutritionRequest::addExcludedIngredient(const std::string& name) {
    if (name.empty())
        throw InvalidDataException(
            "Excluded ingredient name cannot be empty");

    excludedIngredients.insert(normalizeName(name));
}
bool NutritionRequest::removeExcludedIngredient(const std::string& name) {
    return excludedIngredients.erase(normalizeName(name)) > 0;
}
bool NutritionRequest::isIngredientExcluded(const std::string& name) const {
    return excludedIngredients.find(normalizeName(name)) != excludedIngredients.end();
}
const std::set<std::string>& NutritionRequest::getExcludedIngredients() const {
    return excludedIngredients;
}
bool NutritionRequest::hasNutritionLimits() const {
    return maxCalories > 0.0f || minProteins > 0.0f || maxFats > 0.0f || maxCarbs > 0.0f;
}
bool NutritionRequest::hasCriteria() const {
    return hasNutritionLimits() || !excludedIngredients.empty();
}

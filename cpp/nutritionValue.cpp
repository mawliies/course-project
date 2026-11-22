#include "nutritionValue.h"
NutritionValue::NutritionValue(float calories, float protein, float fats, float carbs) {
    this->calories = calories;
    this->protein = protein;
    this->fats = fats;
    this->carbs = carbs;
}
float NutritionValue::getCalories() const{
    return this->calories;
}
float NutritionValue::getProteins() const {
    return this->protein;
}
float NutritionValue::getFats() const {
    return this->fats;
}
float NutritionValue::getCarbs() const {
    return this->carbs;
}
void NutritionValue::setNutrition(float cal, float proteins, float fats, float carbs) {
    this->calories = cal;
    this->protein = proteins;
    this->fats = fats;
    this->carbs = carbs;
}
std::istream& operator>>(std::istream& is, NutritionValue& nutrition) {
    float cals, proteins, fats, carbs;
    //nullptr
    if(is>>cals>>proteins>>fats>>carbs)
        nutrition.setNutrition(cals, proteins, fats, carbs);
    return is;
}
std::ostream& operator<<(std::ostream& os, const NutritionValue& value) {
    os << value.calories << " cal / " << value.protein << " / " << value.fats << " / " << value.carbs;
    return os;
}
NutritionValue::~NutritionValue() {}

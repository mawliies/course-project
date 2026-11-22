#pragma once
#include<iostream>
class NutritionValue {
private:
	float calories;
	float protein;
	float fats;
	float carbs;
public:
	NutritionValue(float calories, float protein, float fats, float carbs);
	friend std::ostream& operator<<(std::ostream& os, const NutritionValue& value);
	friend std::istream& operator>>(std::istream& is, NutritionValue& nutrition);
	float getCalories()const;
	float getProteins()const;
	float getFats()const;
	float getCarbs()const;
	void setNutrition(float cal, float proteins, float fats, float carbs);
	~NutritionValue();
};

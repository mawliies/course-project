#pragma once
#pragma once
#include "RestaurantExceptions.h"
class OutOfRangeException :public RestaurantException {
public:
	explicit OutOfRangeException(int i, int size) :RestaurantException("Index " + std::to_string(i) + " is out of range. Collection size: " + std::to_string(size)) {};
};

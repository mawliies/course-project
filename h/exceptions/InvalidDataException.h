#pragma once
#pragma once
#include"RestaurantExceptions.h"
class InvalidDataException :public RestaurantException {
public:
	explicit InvalidDataException(const std::string& data) :RestaurantException("Incorrect input: " + data) {}
};

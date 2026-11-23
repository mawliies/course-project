#pragma once
#pragma once
#include "RestaurantExceptions.h"
class NotFoundException :public RestaurantException {
public:
	explicit NotFoundException(const std::string& data) :RestaurantException("No object found: " + data) {};
};

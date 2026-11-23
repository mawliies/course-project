#pragma once
#pragma once
#include "RestaurantExceptions.h"
class InvalidOperationException :public RestaurantException {
public:
	explicit InvalidOperationException(const std::string& data) :RestaurantException("Invalid operation error: " + data) {};
};

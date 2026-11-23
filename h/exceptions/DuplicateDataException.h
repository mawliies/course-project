#pragma once
#pragma once
#include "RestaurantExceptions.h"
class DuplicateDataException :public RestaurantException {
public:
	explicit DuplicateDataException(const std::string& data) :RestaurantException("Duplicate data: " + data) {};
};

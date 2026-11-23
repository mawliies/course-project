#pragma once
#include"InvalidDataException.h"
class InvalidPriceException :public InvalidDataException {
public:
	explicit InvalidPriceException(const std::string& data) :InvalidDataException("Invalid price: " + data) {};
};

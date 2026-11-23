#pragma once
#include"InvalidDataException.h"
class InvalidAmountException :public InvalidDataException {
public:
	explicit InvalidAmountException(const std::string& data) :InvalidDataException("Invalid amount: " + data) {};
};
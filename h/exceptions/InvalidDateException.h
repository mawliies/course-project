#pragma once
#include"InvalidDataException.h"
class InvalidDateException :public InvalidDataException {
public:
	explicit InvalidDateException(const std::string& data) :InvalidDataException(" date: " + data) {};
};

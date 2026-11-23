#pragma once
#include"InvalidDataException.h"
class InvalidPhoneNumberException :public InvalidDataException {
public:
	explicit InvalidPhoneNumberException(const std::string& data) :InvalidDataException("phone number: " + data) {};
};

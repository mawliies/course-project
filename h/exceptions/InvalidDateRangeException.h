#pragma once
#include "InvalidDataException.h"
class InvalidDateRangeException :public InvalidDataException {
public:
	explicit InvalidDateRangeException(int dd1, int mm1, int yyyy1, int dd2, int mm2, int yyyy2) :InvalidDataException("invalid date range" + std::to_string(dd1) + "." + std::to_string(mm1) + "." + std::to_string(yyyy1) + " - " + std::to_string(dd2) + "." + std::to_string(mm2) + "." + std::to_string(yyyy2)) {};
};

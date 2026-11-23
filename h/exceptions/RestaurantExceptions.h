#pragma once
#pragma once
#include<stdexcept>
#include<string>
class RestaurantException :public std::runtime_error {
public:
	explicit RestaurantException(const std::string& msg) :std::runtime_error(msg) {}
};

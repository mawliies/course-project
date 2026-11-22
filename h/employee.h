#pragma once
#include"person.h"
class Client;
class Employee:public Person {
private:
	std::string position;
	std::string shift;
public:
	Employee(std::string name, std::string middleName, std::string lastName, std::string phoneNumber, std::string shift, std::string position);
	std::string getPosition()const;
	std::string getShift()const;
	void assignShift(std::string newShift);
	std::string getType()const override;
	void printInfo()const override;
	friend std::ostream& operator<<(std::ostream& os, Employee& object);
	~Employee();
};
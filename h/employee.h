#pragma once
#include"person.h"
class Client;
class Employee:public Person {
private:
	std::string position;
	std::string shift;
public:
	Employee(std::string name, std::string lastName, std::string middleName, std::string phoneNumber, std::string shift, std::string position);
	std::string getPosition()const;
	std::string getShift()const;
	void assignShift(std::string newShift);
	std::string getType()const override;
	void printInfo(std::ostream& os)const override;
	~Employee();
};
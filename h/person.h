#pragma once
#include<iostream>
#include<string>
class Person {
protected:
	std::string name,lastName,middleName;
	std::string phoneNumber;
public:
	Person(std::string name, std::string lastName, std::string middleName, std::string phoneNumber);
	std::string getFullName()const;
	std::string getPhoneNumber()const;
	virtual std::string getType()const = 0;
	virtual void printInfo(std::ostream& os)const;
	virtual ~Person();
};
std::ostream& operator<<(std::ostream& os, const Person& object);

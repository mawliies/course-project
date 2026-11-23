#pragma once
#include"person.h"
class Client :public Person {
private:
	int bonus;
public:
	Client(std::string name, std::string lastName, std::string middleName, std::string phoneNumber, int bonus);
	int getBonus()const;
	void setBonus(int bonus);
	std::string getType()const override;
	void printInfo(std::ostream& os)const override;
	~Client();
};
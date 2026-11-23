#pragma once
#include"menu.h"
#include<ctime>
class SeasonalMenu :public Menu {
private:
	int ddBegin, mmBegin, yyyyBegin;
	int ddEnd, mmEnd, yyyyEnd;
public:
	SeasonalMenu();
	int getDayBegin()const;
	int getMonthBegin()const;
	int getYearBegin()const;
	int getDayEnd()const;
	int getMonthEnd()const;
	int getYearEnd()const;
	void setBeginDate(int ddBegin, int mmBegin, int yyyyBegin);
	void setEndDate(int ddEnd, int mmEnd, int yyyyEnd);
	void printMenu()const override;
	bool isAvailable()const override;
	~SeasonalMenu();
};
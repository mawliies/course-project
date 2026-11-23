#include "seasonalMenu.h"
#include "InvalidDateException.h"
#include "InvalidDateRangeException.h"
static bool isDateAfter(int dd1, int mm1, int yyyy1, int dd2, int mm2, int yyyy2) {
	if (yyyy1 != yyyy2)
		return yyyy1 > yyyy2;
	if (mm1 != mm2)
		return mm1 > mm2;
	return dd1 > dd2;
}
static void validateDate(int dd, int mm, int yyyy) {
	if (yyyy <= 0 || mm < 1 || mm > 12 || dd < 1 || dd > daysInMonth(mm, yyyy)) 
		throw InvalidDateException(std::to_string(dd) + "." + std::to_string(mm) + "." + std::to_string(yyyy));
}

SeasonalMenu::SeasonalMenu(): ddBegin(0), mmBegin(0), yyyyBegin(0),ddEnd(0), mmEnd(0), yyyyEnd(0) {}
SeasonalMenu::SeasonalMenu(int ddBegin, int mmBegin, int yyyyBegin, int ddEnd, int mmEnd, int yyyyEnd) {
	this->ddBegin = ddBegin;
	this->mmBegin = mmBegin;
	this->yyyyBegin = yyyyBegin;
	this->ddEnd = ddEnd;
	this->mmEnd = mmEnd;
	this->yyyyEnd = yyyyEnd;
}
int SeasonalMenu::getDayBegin()const {
	return this->ddBegin;
}
int SeasonalMenu::getMonthBegin()const {
	return this->mmBegin;
}
int SeasonalMenu::getYearBegin()const {
	return this->yyyyBegin;
}
int SeasonalMenu::getDayEnd()const {
	return this->ddEnd;
}
int SeasonalMenu::getMonthEnd()const {
	return this->mmEnd;
}
int SeasonalMenu::getYearEnd()const {
	return this->yyyyEnd;
}
void SeasonalMenu::setBeginDate(int ddBegin, int mmBegin, int yyyyBegin) {
	validateDate(ddBegin, mmBegin, yyyyBegin);
	if (ddEnd != 0 && isDateAfter(ddBegin, mmBegin, yyyyBegin, ddEnd, mmEnd, yyyyEnd)) 
		throw InvalidDateRangeException(ddBegin, mmBegin, yyyyBegin, ddEnd, mmEnd, yyyyEnd);
	
	this->ddBegin = ddBegin;
	this->mmBegin = mmBegin;
	this->yyyyBegin = yyyyBegin;
}
void SeasonalMenu::setEndDate(int ddEnd, int mmEnd, int yyyyEnd) {
	validateDate(ddEnd, mmEnd, yyyyEnd);
	if (ddBegin != 0 && isDateAfter(ddBegin, mmBegin, yyyyBegin, ddEnd, mmEnd, yyyyEnd))
		throw InvalidDateRangeException(ddBegin, mmBegin, yyyyBegin, ddEnd, mmEnd, yyyyEnd);
	
	this->ddEnd = ddEnd;
	this->mmEnd = mmEnd;
	this->yyyyEnd = yyyyEnd;
}
void SeasonalMenu::printMenu()const {
	std::cout << "\n---SEASONAL MENU---\n";
	Menu::printMenu();
	std::cout << "\n\n\n"<< this->getDayBegin() << "." << this->getMonthBegin() << "." << this->getYearBegin()<<" - "<< this->getDayEnd() << "." << this->getMonthEnd() << "." << this->getYearEnd();
}
bool SeasonalMenu::isAvailable()const {
	if (ddBegin == 0 || ddEnd == 0)
		return false;
	std::time_t now = std::time(nullptr);
	std::tm currentTime{};
	localtime_s(&currentTime, &now);
	int currentDay = currentTime.tm_mday;
	int currentMonth = currentTime.tm_mon + 1;
	int currentYear = currentTime.tm_year + 1900;
	bool afterBegin = !isDateAfter(ddBegin, mmBegin, yyyyBegin, currentDay, currentMonth, currentYear);
	bool beforeEnd = !isDateAfter(currentDay, currentMonth, currentYear, ddEnd, mmEnd, yyyyEnd);
	return afterBegin && beforeEnd;
}
SeasonalMenu::~SeasonalMenu() {}
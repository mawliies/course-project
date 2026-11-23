#include "seasonalMenu.h"
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
	this->ddBegin = ddBegin;
	this->mmBegin = mmBegin;
	this->yyyyBegin = yyyyBegin;
}
void SeasonalMenu::setEndDate(int ddEnd, int mmEnd, int yyyyEnd) {
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
	std::time_t now = std::time(nullptr);
	const std::tm* currentDate = std::localtime(&now);

	if (currentDate == nullptr)
		return false;//nullptr

	int today = (currentDate->tm_year + 1900) * 10000
		+ (currentDate->tm_mon + 1) * 100
		+ currentDate->tm_mday;

	int begin = yyyyBegin * 10000 + mmBegin * 100 + ddBegin;
	int end = yyyyEnd * 10000 + mmEnd * 100 + ddEnd;

	return today >= begin && today <= end;
}
SeasonalMenu::~SeasonalMenu() {}
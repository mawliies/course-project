#pragma once
#include "restaurantManager.h"
#include<vector>
#include <string>
#include <fstream>

class FileManager {
private:
    std::string databaseFile;
    std::string logFile;
    std::string reportFile;
public:
    FileManager(const std::string& databaseFile, const std::string& logFile, const std::string& reportFile);

    void writeHeader(std::ofstream& file) const;
    void writeIngredients(std::ofstream& file, const IngredientCatalog& catalog) const;
    void writeMainMenu(std::ofstream& file, const MainMenu& menu) const;
    void writeSeasonalMenu(std::ofstream& file, const SeasonalMenu& menu) const;
    void writePeople(std::ofstream& file, const Collection<Person*>& people) const;
    void writeOrders(std::ofstream& file, const std::vector<Order>& orders) const;
    void writeMenu(std::ofstream& file, const Menu& menu) const;
    void writeRecipe(std::ofstream& file, const Recipe& recipe) const;
    void writeDiningRoom(std::ofstream& file, const DiningRoom& diningRoom) const;
    void saveDatabase(const RestaurantManager& manager) const;

    int readHeader(std::ifstream& file) const;
    void readIngredients(std::ifstream& file, RestaurantManager& manager) const;
    void readMainMenu(std::ifstream& file, RestaurantManager& manager) const;
    void readSeasonalMenu(std::ifstream& file, RestaurantManager& manager) const;
    void readPeople(std::ifstream& file, RestaurantManager& manager) const;
    void readOrders(std::ifstream& file, RestaurantManager& manager, int version) const;
    void readMenu(std::ifstream& file, RestaurantManager& manager, bool isMainMenu) const;
    Recipe readRecipe(std::ifstream& file, const IngredientCatalog& catalog) const;
    void readDiningRoom(std::ifstream& file, RestaurantManager& manager) const;
    void loadDatabase(RestaurantManager& manager) const;

    void writeLog(const std::string& action) const;
    void generateReport(const RestaurantManager& manager) const;
};

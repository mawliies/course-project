#pragma once
#include "diningTable.h"
#include <map>
#include <vector>
#include <cstddef>

class DiningRoom {
private:
    std::map<int, DiningTable> tables;
public:
    void addTable(const DiningTable& table);
    void addTable(int number, int capacity);
    void removeTable(int number);
    DiningTable* findTable(int number);
    const DiningTable* findTable(int number) const;
    DiningTable* findAvailableTable(int guests);
    const DiningTable* findAvailableTable(int guests) const;
    void occupyTable(int number, int guests);
    void releaseTable(int number);
    std::vector<const DiningTable*> getAvailableTables(int guests = 1) const;
    const std::map<int, DiningTable>& getTables() const;
    std::size_t getTableCount() const;
    std::size_t getAvailableTableCount() const;
    std::size_t getOccupiedTableCount() const;
};
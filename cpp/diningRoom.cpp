#include "diningRoom.h"
#include "AllExceptions.h"
#include <algorithm>
#include <string>
#include <utility>

void DiningRoom::addTable(const DiningTable& table) {
    const int number = table.getNumber();
    if (tables.find(number) != tables.end())
        throw DuplicateDataException("Table number: " + std::to_string(number));
    if (!table.isAvailable() || table.getOccupiedSeats() != 0)
        throw InvalidOperationException("Cannot add an already occupied table");
    tables.insert(std::make_pair(number, table));
}
void DiningRoom::addTable(int number, int capacity) {
    addTable(DiningTable(number, capacity));
}
void DiningRoom::removeTable(int number) {
    auto found = tables.find(number);
    if (found == tables.end())
        throw NotFoundException("Table number: " + std::to_string(number));
    if (!found->second.isAvailable())
        throw InvalidOperationException("Cannot remove an occupied table");
    tables.erase(found);
}

DiningTable* DiningRoom::findTable(int number) {
    auto found = tables.find(number);
    return found == tables.end() ? nullptr : &found->second;
}
const DiningTable* DiningRoom::findTable(int number) const {
    auto found = tables.find(number);
    return found == tables.end() ? nullptr : &found->second;
}

DiningTable* DiningRoom::findAvailableTable(int guests) {
    if (guests <= 0)
        throw InvalidAmountException("Guest count must be positive");
    DiningTable* best = nullptr;
    for (auto& entry : tables) {
        DiningTable& table = entry.second;
        if (table.canAccommodate(guests) && (best == nullptr || table.getCapacity() < best->getCapacity()))
            best = &table;
    }
    return best;
}

const DiningTable* DiningRoom::findAvailableTable(int guests) const {
    if (guests <= 0)
        throw InvalidAmountException("Guest count must be positive");
    const DiningTable* best = nullptr;
    for (const auto& entry : tables) {
        const DiningTable& table = entry.second;
        if (table.canAccommodate(guests) && (best == nullptr ||  table.getCapacity() < best->getCapacity())) 
            best = &table;
    }
    return best;
}

void DiningRoom::occupyTable(int number, int guests) {
    DiningTable* table = findTable(number);
    if (table == nullptr)
        throw NotFoundException("Table number: " + std::to_string(number));
    table->occupy(guests);
}

void DiningRoom::releaseTable(int number) {
    DiningTable* table = findTable(number);
    if (table == nullptr)
        throw NotFoundException("Table number: " + std::to_string(number));
    table->release();
}

std::vector<const DiningTable*> DiningRoom::getAvailableTables(int guests) const {
    if (guests <= 0)
        throw InvalidAmountException("Guest count must be positive");
    std::vector<const DiningTable*> result;
    for (const auto& entry : tables) 
        if (entry.second.canAccommodate(guests))
            result.push_back(&entry.second);
    return result;
}

const std::map<int, DiningTable>& DiningRoom::getTables() const {
    return tables;
}

std::size_t DiningRoom::getTableCount() const {
    return tables.size();
}

std::size_t DiningRoom::getAvailableTableCount() const {
    return static_cast<std::size_t>(std::count_if(tables.begin(),tables.end(),[](const std::pair<const int, DiningTable>& entry) {
        return entry.second.isAvailable();
    }
    )
    );
}

std::size_t DiningRoom::getOccupiedTableCount() const {
    return tables.size() - getAvailableTableCount();
}
#include "diningTable.h"
#include "AllExceptions.h"

DiningTable::DiningTable(int number, int capacity) : number(number), capacity(capacity), occupiedSeats(0), status(DiningTableStatus::Available) {
    if (number <= 0)
        throw InvalidDataException("Table number must be positive");
    if (capacity <= 0)
        throw InvalidDataException("Table capacity must be positive");
}
int DiningTable::getNumber() const {
    return number;
}
int DiningTable::getCapacity() const {
    return capacity;
}
int DiningTable::getOccupiedSeats() const {
    return occupiedSeats;
}
int DiningTable::getAvailableSeats() const {
    return capacity - occupiedSeats;
}
DiningTableStatus DiningTable::getStatus() const {
    return status;
}
bool DiningTable::isAvailable() const {
    return status == DiningTableStatus::Available;
}
bool DiningTable::canAccommodate(int guests) const {
    return guests > 0 && isAvailable() && guests <= capacity;
}
void DiningTable::occupy(int guests) {
    if (guests <= 0)
        throw InvalidAmountException("Guest count must be positive");
    if (!isAvailable())
        throw InvalidOperationException("Table is already occupied");
    if (guests > capacity)
        throw InvalidOperationException("Guest count exceeds table capacity");
    occupiedSeats = guests;
    status = DiningTableStatus::Occupied;
}

void DiningTable::release() {
    if (isAvailable())
        throw InvalidOperationException("Table is already available");
    occupiedSeats = 0;
    status = DiningTableStatus::Available;
}
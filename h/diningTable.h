#pragma once
enum class DiningTableStatus {
    Available,
    Occupied
};

class DiningTable {
private:
    int number;
    int capacity;
    int occupiedSeats;
    DiningTableStatus status;

public:
    DiningTable(int number, int capacity);
    int getNumber() const;
    int getCapacity() const;
    int getOccupiedSeats() const;
    int getAvailableSeats() const;
    DiningTableStatus getStatus() const;
    bool isAvailable() const;
    bool canAccommodate(int guests) const;
    void occupy(int guests);
    void release();
};
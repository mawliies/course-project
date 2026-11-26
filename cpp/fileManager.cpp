#include <ctime>
#include "fileManager.h"
#include "dish.h"
#include "drink.h"
#include "client.h"
#include "employee.h"
#include "AllExceptions.h"
#include <iomanip>
#include <limits>
#include <fstream>

FileManager::FileManager( const std::string& databaseFile,const std::string& logFile,const std::string& reportFile) : databaseFile(databaseFile),logFile(logFile),reportFile(reportFile) {}
void FileManager::writeHeader(std::ofstream& file) const {
    file << "RESTAURANT_DATABASE\n";
    file << "VERSION 3\n";
}
void FileManager::writeIngredients(std::ofstream& file, const IngredientCatalog& catalog) const {
    const auto& ingredients = catalog.getAll();

    file << "INGREDIENTS\n";
    file << catalog.getNextId() << '\n';
    file << ingredients.size() << '\n';

    for (const Ingredient& ingredient : ingredients) {
        const NutritionValue nutrition = ingredient.getNutrition();

        file << ingredient.getId() << '\n';
        file << std::quoted(ingredient.getName()) << '\n';
        file << nutrition.getCalories() << ' ' << nutrition.getProteins() << ' ' << nutrition.getFats() << ' ' << nutrition.getCarbs() << '\n';
        file << static_cast<int>(ingredient.getUnit()) << '\n';
    }
}
void FileManager::writeRecipe(std::ofstream& file, const Recipe& recipe) const {
    file << recipe.getPortions() << '\n';
    const auto& ingredients = recipe.getIngredients();
    file << ingredients.size() << '\n';
    for (const RecipeIngredient& ingredient : ingredients)
        file << ingredient.ingredientId << ' ' << ingredient.amount << ' ' << std::quoted(ingredient.name) << '\n';
}
void FileManager::writeMenu(std::ofstream& file, const Menu& menu) const {
    const auto items = menu.getItems();
    file << items.size() << '\n';
    for (const MenuObject* object : items) {
        if (object == nullptr)
            throw InvalidDataException("Null menu object");

        file << std::quoted(object->getType()) << '\n';
        file << std::quoted(object->getTitle()) << '\n';
        file << object->getPrice() << '\n';
        file << std::quoted(object->getCategory()) << '\n';

        writeRecipe(file, object->getRecipe());
        if (const Dish* dish = dynamic_cast<const Dish*>(object)) 
            file << dish->getTime() << ' '<< dish->getWeight() << '\n';
        else if (const Drink* drink = dynamic_cast<const Drink*>(object)) 
            file << drink->getVolume() << ' '<< drink->getIsCold() << '\n';
        else 
            throw InvalidDataException("Unknown menu object type");
        
    }
}
void FileManager::writeMainMenu(std::ofstream& file, const MainMenu& menu) const {
    file << "MAIN_MENU\n";
    file << menu.getDay() << ' ' << menu.getMonth() << ' ' << menu.getYear() << '\n';
    writeMenu(file, menu);
}

void FileManager::writeSeasonalMenu(std::ofstream& file, const SeasonalMenu& menu) const {
    file << "SEASONAL_MENU\n";
    file << menu.getDayBegin() << ' ' << menu.getMonthBegin() << ' ' << menu.getYearBegin() << ' ' << menu.getDayEnd() << ' ' << menu.getMonthEnd() << ' ' << menu.getYearEnd() << '\n';
    writeMenu(file, menu);
}

void FileManager::writePeople(std::ofstream& file, const Collection<Person*>& people) const {
    file << "PEOPLE\n";
    file << people.getSize() << '\n';

    for (size_t i = 0; i < people.getSize(); i++) {
        const Person* person = people.get(i);

        if (person == nullptr)
            throw InvalidDataException("Null person in collection");

        file << std::quoted(person->getType()) << '\n';
        file << std::quoted(person->getFirstName()) << '\n';
        file << std::quoted(person->getLastName()) << '\n';
        file << std::quoted(person->getMiddlelName()) << '\n';
        file << std::quoted(person->getPhoneNumber()) << '\n';

        if (const Client* client = dynamic_cast<const Client*>(person)) 
            file << client->getBonus() << '\n';
        else if (const Employee* employee = dynamic_cast<const Employee*>(person)) {
            file << std::quoted(employee->getShift()) << '\n';
            file << std::quoted(employee->getPosition()) << '\n';
        }
        else 
            throw InvalidDataException("Unknown person type");
        
    }
}

void FileManager::writeOrders(std::ofstream& file, const std::vector<Order>& orders) const {
    file << "ORDERS\n";
    file << orders.size() << '\n';

    for (const Order& order : orders) {
        file << std::quoted(order.getClientPhoneNumber()) << '\n';
        file << static_cast<int>(order.getStatus()) << '\n';
        file << order.getGuestCount() << ' ' << order.getTableNumber() << '\n';
        file << order.getLoyaltyDiscount() << ' ' << order.getRedeemedBonusPoints() << '\n';
        const auto& items = order.getItems();
        file << items.size() << '\n';

        for (const OrderItem& item : items) {
            file << std::quoted(item.name) << '\n';
            file << item.amount << ' ' << item.priceAtOrder << '\n';
        }
    }
}
void FileManager::writeDiningRoom(std::ofstream& file, const DiningRoom& diningRoom) const {
    file << "DINING_ROOM\n";
    const auto& tables = diningRoom.getTables();
    file << tables.size() << '\n';
    for (const auto& entry : tables) 
        file << entry.second.getNumber() << ' '<< entry.second.getCapacity() << '\n';
}
void FileManager::saveDatabase(const RestaurantManager& manager) const {
    std::ofstream file(databaseFile);

    if (!file.is_open())
        throw InvalidOperationException("Cannot open database file for writing: " + databaseFile);

    // Preserve exact floating-point values for recipe nutrition, historical
    // prices and loyalty discounts across save/load cycles.
    file << std::setprecision(std::numeric_limits<float>::max_digits10);
    writeHeader(file);
    writeIngredients(file, manager.getCatalog());
    writeMainMenu(file, manager.getMainMenu());
    writeSeasonalMenu(file, manager.getSeasonalMenu());
    writeDiningRoom(file, manager.getDiningRoom());
    writePeople(file, manager.getPeople());
    writeOrders(file, manager.getOrders());
    

    file.flush();
    if (!file)
        throw InvalidOperationException("Error while writing database file");
    file.close();
    if (file.fail())
        throw InvalidOperationException("Error while closing database file");
}
int FileManager::readHeader(std::ifstream& file) const {
    std::string header;
    std::string versionLabel;
    int version;

    if (!(file >> header >> versionLabel >> version))
        throw InvalidDataException("Cannot read database header");
    if (header != "RESTAURANT_DATABASE" || versionLabel != "VERSION" || (version < 1 || version > 3))
        throw InvalidDataException("Invalid database format or version");
    return version;
}
void FileManager::readIngredients(std::ifstream& file, RestaurantManager& manager) const {
    std::string section;
    int nextId;
    size_t count;

    if (!(file >> section) || section != "INGREDIENTS")
        throw InvalidDataException("Invalid ingredients section");
    if (!(file >> nextId >> count))
        throw InvalidDataException("Cannot read ingredients information");
    for (size_t i = 0; i < count; i++) {
        int id;
        std::string name;
        float calories, proteins, fats, carbs;
        int unitValue;

        if (!(file >> id >> std::quoted(name) >> calories >> proteins >> fats >> carbs >> unitValue))
            throw InvalidDataException("Cannot read ingredient");
        if (unitValue < 0 || unitValue > 1)
            throw InvalidDataException("Invalid ingredient measure unit");
        if (calories < 0 || proteins < 0 || fats < 0 || carbs < 0)
            throw InvalidDataException("Invalid nutrition values");

        NutritionValue nutrition(calories, proteins, fats, carbs);
        MeasureUnit unit = static_cast<MeasureUnit>(unitValue);

        Ingredient ingredient(id, name, nutrition, unit);
        manager.addIngredient(ingredient);
    }
    manager.setNextIngredientId(nextId);
}
Recipe FileManager::readRecipe(std::ifstream& file, const IngredientCatalog& catalog) const {
    int portions;
    size_t count;

    if (!(file >> portions >> count))
        throw InvalidDataException("Cannot read recipe information");

    Recipe recipe(portions);
    for (size_t i = 0; i < count; i++) {
        int ingredientId;
        float amount;
        std::string name;

        if (!(file >> ingredientId >> amount >> std::quoted(name)))
            throw InvalidDataException("Cannot read recipe ingredient");
        const Ingredient* ingredient = catalog.findById(ingredientId);
        if (ingredient == nullptr || ingredient->getName() != name)
            throw InvalidDataException("Recipe ingredient does not match catalog");

        recipe.addIngredient(amount, name, catalog);
    }

    return recipe;
}
void FileManager::readMenu(std::ifstream& file, RestaurantManager& manager, bool isMainMenu) const {
    size_t count;

    if (!(file >> count))
        throw InvalidDataException("Cannot read menu size");

    for (size_t i = 0; i < count; i++) {
        std::string type;
        std::string title;
        std::string category;
        float price;

        if (!(file >> std::quoted(type) >> std::quoted(title) >> price >> std::quoted(category)))
            throw InvalidDataException("Cannot read menu object");
        Recipe recipe = readRecipe(file, manager.getCatalog());
        MenuObject* object = nullptr;
        if (type == "Dish") {
            int cookingTime;
            int weight;
            if (!(file >> cookingTime >> weight))
                throw InvalidDataException("Cannot read dish information");
            object = new Dish(title, price, category, recipe, cookingTime, weight);
        }
        else if (type == "Drink") {
            int volume;
            int coldValue;
            if (!(file >> volume >> coldValue))
                throw InvalidDataException("Cannot read drink information");
            if (coldValue != 0 && coldValue != 1)
                throw InvalidDataException("Invalid drink temperature value");
            object = new Drink(title, price, category, recipe,volume, coldValue == 1);
        }
        else 
            throw InvalidDataException("Unknown menu object type: " + type);
        try {
            if (isMainMenu)
                manager.addToMainMenu(object);
            else
                manager.addToSeasonalMenu(object);
        }
        catch (...) {
            delete object;
            throw;
        }
    }
}
void FileManager::readMainMenu(std::ifstream& file,RestaurantManager& manager) const {
    std::string section;
    int day, month, year;
    if (!(file >> section) || section != "MAIN_MENU")
        throw InvalidDataException("Invalid main menu section");
    if (!(file >> day >> month >> year))
        throw InvalidDataException("Cannot read main menu date");
    if (day != 0 || month != 0 || year != 0)
        manager.setMainMenuUpdateDate(day, month, year);
    readMenu(file, manager, true);
}
void FileManager::readSeasonalMenu(std::ifstream& file,RestaurantManager& manager) const {
    std::string section;
    int dayBegin, monthBegin, yearBegin;
    int dayEnd, monthEnd, yearEnd;
    if (!(file >> section) || section != "SEASONAL_MENU")
        throw InvalidDataException("Invalid seasonal menu section");
    if (!(file >> dayBegin >> monthBegin >> yearBegin >> dayEnd >> monthEnd >> yearEnd))
        throw InvalidDataException("Cannot read seasonal menu dates");
    if (dayBegin != 0 || monthBegin != 0 || yearBegin != 0 ||dayEnd != 0 || monthEnd != 0 || yearEnd != 0) 
        manager.setSeasonalMenuPeriod(dayBegin, monthBegin, yearBegin,dayEnd, monthEnd, yearEnd);
    readMenu(file, manager, false);
}
void FileManager::readPeople(std::ifstream& file,RestaurantManager& manager) const {
    std::string section;
    size_t count;

    if (!(file >> section) || section != "PEOPLE")
        throw InvalidDataException("Invalid people section");
    if (!(file >> count))
        throw InvalidDataException("Cannot read people count");
    for (size_t i = 0; i < count; i++) {
        std::string type;
        std::string firstName;
        std::string lastName;
        std::string middleName;
        std::string phoneNumber;
        if (!(file >> std::quoted(type) >> std::quoted(firstName) >> std::quoted(lastName) >> std::quoted(middleName) >> std::quoted(phoneNumber)))
            throw InvalidDataException("Cannot read person information");
        Person* person = nullptr;
        if (type == "Client") {
            int bonus;
            if (!(file >> bonus))
                throw InvalidDataException("Cannot read client bonus");
            person = new Client(firstName, lastName, middleName, phoneNumber, bonus);
        }
        else if (type == "Employee") {
            std::string shift;
            std::string position;
            if (!(file >> std::quoted(shift) >> std::quoted(position)))
                throw InvalidDataException("Cannot read employee information");
            person = new Employee(firstName, lastName, middleName, phoneNumber, shift, position);
        }
        else 
            throw InvalidDataException("Unknown person type: " + type);
        try {
            manager.addPerson(person);
        }
        catch (...) {
            delete person;
            throw;
        }
    }
}
void FileManager::readDiningRoom(std::ifstream& file,RestaurantManager& manager) const {
    std::string section;
    size_t count;
    if (!(file >> section) || section != "DINING_ROOM")
        throw InvalidDataException("Invalid dining room section");
    if (!(file >> count))
        throw InvalidDataException("Cannot read dining room table count");
    for (size_t i = 0; i < count; i++) {
        int number;
        int capacity;
        if (!(file >> number >> capacity))
            throw InvalidDataException("Cannot read dining table information");
        manager.addTable(number, capacity);
    }
}
void FileManager::readOrders(std::ifstream& file, RestaurantManager& manager, int version) const {
    std::string section;
    size_t count;

    if (!(file >> section) || section != "ORDERS")
        throw InvalidDataException("Invalid orders section");
    if (!(file >> count))
        throw InvalidDataException("Cannot read orders count");
    for (size_t i = 0; i < count; i++) {
        std::string clientPhoneNumber;
        int statusValue;
        if (!(file >> std::quoted(clientPhoneNumber) >> statusValue))
            throw InvalidDataException("Cannot read order information");
        if (statusValue < 0 || statusValue > 5)
            throw InvalidDataException("Invalid order status");

        int guestCount = 1;
        int tableNumber = 0;
        if (version >= 2) 
            if (!(file >> guestCount >> tableNumber))
                throw InvalidDataException("Cannot read order seating information");
        if (guestCount <= 0 || tableNumber < 0)
            throw InvalidDataException("Invalid order seating information");

        float loyaltyDiscount = 0.0f;
        int redeemedBonusPoints = 0;
        if (version >= 3 && !(file >> loyaltyDiscount >> redeemedBonusPoints))
            throw InvalidDataException("Cannot read order loyalty redemption");
        if (loyaltyDiscount < 0.0f || redeemedBonusPoints < 0)
            throw InvalidDataException("Invalid order loyalty redemption");

        Order order(clientPhoneNumber,static_cast<OrderStatus>(statusValue), guestCount);
        size_t itemsCount;
        if (!(file >> itemsCount))
            throw InvalidDataException("Cannot read order items count");
        for (size_t j = 0; j < itemsCount; j++) {
            OrderItem item;
            if (!(file >> std::quoted(item.name)>> item.amount >> item.priceAtOrder)) 
                throw InvalidDataException("Cannot read order item");
            order.restore(item);
        }
        if (version >= 3)
            order.applyLoyaltyRedemption(redeemedBonusPoints, loyaltyDiscount);
        const int orderIndex = manager.getOrdersCount();
        manager.addOrder(order);
        if (tableNumber > 0)
            manager.assignTableToOrder(orderIndex, tableNumber);
    }
}
void FileManager::loadDatabase(RestaurantManager& manager) const {
    std::ifstream file(databaseFile);
    if (!file.is_open())
        throw InvalidOperationException("Cannot open database file for reading: " + databaseFile);
    const MainMenu& mainMenu = manager.getMainMenu();
    const SeasonalMenu& seasonalMenu = manager.getSeasonalMenu();

    if (!manager.getCatalog().getAll().empty() || !manager.getDiningRoom().getTables().empty() || !mainMenu.getItems().empty() || !seasonalMenu.getItems().empty() || manager.getPeople().getSize() != 0 || !manager.getOrders().empty() || mainMenu.getDay() != 0 || mainMenu.getMonth() != 0 || mainMenu.getYear() != 0 || seasonalMenu.getDayBegin() != 0 || seasonalMenu.getMonthBegin() != 0 || seasonalMenu.getYearBegin() != 0 || seasonalMenu.getDayEnd() != 0 || seasonalMenu.getMonthEnd() != 0 || seasonalMenu.getYearEnd() != 0)
        throw InvalidOperationException("Database must be loaded into an empty RestaurantManager");
    const int version = readHeader(file);
    readIngredients(file, manager);
    readMainMenu(file, manager);
    readSeasonalMenu(file, manager);
    if (version >= 2)
        readDiningRoom(file, manager);
    readPeople(file, manager);
    readOrders(file, manager, version);
    file >> std::ws;

    if (file.peek() != std::char_traits<char>::eof())
        throw InvalidDataException("Unexpected data at the end of database file");
    if (file.bad())
        throw InvalidOperationException("Error while reading database file");

    file.close();
}
void FileManager::writeLog(const std::string& action) const {
    std::ofstream file(logFile, std::ios::app);
    if (!file.is_open())
        throw InvalidOperationException("Cannot open log file: " + logFile);
    std::time_t currentTime = std::time(nullptr);
    std::tm localTime{};
    if (localtime_s(&localTime, &currentTime) != 0)
        throw InvalidOperationException("Cannot get current date and time");

    file << '[' << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] " << action << '\n';
    file.flush();
    if (!file)
        throw InvalidOperationException("Error while writing log file");
    file.close();
    if (file.fail())
        throw InvalidOperationException("Error while closing log file");
}
void FileManager::generateReport(const RestaurantManager& manager) const {
    std::ofstream file(reportFile);
    if (!file.is_open())
        throw InvalidOperationException("Cannot open report file: " + reportFile);
    std::time_t currentTime = std::time(nullptr);
    std::tm localTime{};
    if (localtime_s(&localTime, &currentTime) != 0)
        throw InvalidOperationException("Cannot get current date and time");

    file << "========================================\n";
    file << "         RESTAURANT REPORT\n";
    file << "========================================\n";

    file << "Generated: "<< std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "\n\n";
    file << std::fixed << std::setprecision(2);
    
    file << "========================================\n";
    file << "          INGREDIENT CATALOG\n";
    file << "========================================\n";
    const auto& ingredients = manager.getCatalog().getAll();
    file << "Total ingredients: " << ingredients.size() << '\n';
    file << "Next available ID: " << manager.getCatalog().getNextId() << "\n\n";
    for (const Ingredient& ingredient : ingredients) {
        const NutritionValue nutrition = ingredient.getNutrition();
        std::string unit;
        switch (ingredient.getUnit()) {
        case MeasureUnit::gram:
            unit = "g";
            break;
        case MeasureUnit::milliliter:
            unit = "ml";
            break;
        default:
            unit = "unknown";
        }

        file << "ID: " << ingredient.getId() << '\n';
        file << "Name: " << ingredient.getName() << '\n';
        file << "Unit: " << unit << '\n';
        file << "Nutrition per 100 units:\n";
        file << "  Calories: " << nutrition.getCalories() << '\n';
        file << "  Proteins: " << nutrition.getProteins() << '\n';
        file << "  Fats: " << nutrition.getFats() << '\n';
        file << "  Carbs: " << nutrition.getCarbs() << '\n';
        file << "----------------------------------------\n";
    }

    const MainMenu& mainMenu = manager.getMainMenu();
    file << "\n========================================\n";
    file << "              MAIN MENU\n";
    file << "========================================\n";
    file << "Last update: " << mainMenu.getDay() << '.' << mainMenu.getMonth() << '.' << mainMenu.getYear() << '\n';

    const SeasonalMenu& seasonalMenu = manager.getSeasonalMenu();
    file << "\n========================================\n";
    file << "            SEASONAL MENU\n";
    file << "========================================\n";
    file << "Period: " << seasonalMenu.getDayBegin() << '.' << seasonalMenu.getMonthBegin() << '.' << seasonalMenu.getYearBegin() << " - " << seasonalMenu.getDayEnd() << '.' << seasonalMenu.getMonthEnd() << '.' << seasonalMenu.getYearEnd() << '\n';
    file << "Currently available: " << (manager.isSeasonalMenuAvailable() ? "Yes" : "No") << '\n';

    auto writeMenuItems = [&file](const Menu& menu) {
        const auto items = menu.getItems();
        file << "Total items: " << items.size() << "\n\n";
        for (const MenuObject* object : items) {
            if (object == nullptr) {
                file << "Invalid menu object\n";
                continue;
            }
            file << "Type: " << object->getType() << '\n';
            file << "Name: " << object->getTitle() << '\n';
            file << "Category: " << object->getCategory() << '\n';
            file << "Price: " << object->getPrice() << '\n';
            if (const Dish* dish = dynamic_cast<const Dish*>(object)) {
                file << "Cooking time: " << dish->getTime() << " min\n";
                file << "Weight: " << dish->getWeight() << " g\n";
            }
            else if (const Drink* drink = dynamic_cast<const Drink*>(object)) {
                file << "Volume: " << drink->getVolume() << " ml\n";
                file << "Temperature: " << (drink->getIsCold() ? "Cold" : "Hot") << '\n';
            }

            const Recipe& recipe = object->getRecipe();
            file << "Recipe portions: " << recipe.getPortions() << '\n';
            const auto& recipeIngredients = recipe.getIngredients();
            file << "Recipe ingredients:\n";
            if (recipeIngredients.empty())
                file << "  No ingredients\n";
            else
                for (const RecipeIngredient& ingredient : recipeIngredients)
                    file << "  " << ingredient.name << " (ID: " << ingredient.ingredientId << ") - " << ingredient.amount << '\n';
            file << "----------------------------------------\n";
        }
    };
    file << "\nMain menu items:\n";
    writeMenuItems(mainMenu);
    file << "\nSeasonal menu items:\n";
    writeMenuItems(seasonalMenu);

    file << "\n========================================\n";
    file << "              DINING ROOM\n";
    file << "========================================\n";
    const auto& tables = manager.getDiningRoom().getTables();
    file << "Total tables: " << tables.size() << '\n';
    file << "Available tables: "<< manager.getDiningRoom().getAvailableTableCount() << '\n';
    file << "Occupied tables: " << manager.getDiningRoom().getOccupiedTableCount() << "\n\n";
    for (const auto& entry : tables) {
        const DiningTable& table = entry.second;
        file << "Table #" << table.getNumber() << '\n';
        file << "Capacity: " << table.getCapacity() << '\n';
        file << "Occupied seats: " << table.getOccupiedSeats() << '\n';
        file << "Status: " << (table.isAvailable() ? "Available" : "Occupied") << '\n';
        file << "----------------------------------------\n";
    }

    file << "\n========================================\n";
    file << "                PEOPLE\n";
    file << "========================================\n";
    const Collection<Person*>& people = manager.getPeople();
    file << "Total people: " << people.getSize() << "\n\n";
    if (people.getSize() == 0) 
        file << "No registered people.\n";
    else 
        manager.printPeople(file);
    
    file << "\n========================================\n";
    file << "                ORDERS\n";
    file << "========================================\n";
    const auto& orders = manager.getOrders();
    file << "Total orders: " << orders.size() << "\n\n";
    for (size_t i = 0; i < orders.size(); i++) {
        const Order& order = orders[i];
        std::string status;
        switch (order.getStatus()) {
        case OrderStatus::Draft:
            status = "Draft";
            break;
        case OrderStatus::New:
            status = "New";
            break;
        case OrderStatus::Ready:
            status = "Ready";
            break;
        case OrderStatus::Served:
            status = "Served";
            break;
        case OrderStatus::Cancelled:
            status = "Cancelled";
            break;
        case OrderStatus::Preparing:
            status = "Preparing";
            break;
        default:
            throw InvalidDataException("Unknown order status");
        }
        file << "Order #" << i + 1 << '\n';
        file << "Client phone: " << order.getClientPhoneNumber() << '\n';
        file << "Status: " << status << '\n';
        file << "Items:\n";
        const auto& items = order.getItems();
        if (items.empty()) 
            file << "  No items\n";
        else 
            for (const OrderItem& item : items) {
                file << "  " << item.name << '\n';
                file << "    Amount: " << item.amount << '\n';
                file << "    Price at order: " << item.priceAtOrder << '\n';
                file << "    Subtotal: " << item.amount * item.priceAtOrder << '\n';
            }
        file << "Subtotal: " << order.calculateTotal() << '\n';
        file << "Loyalty points redeemed: " << order.getRedeemedBonusPoints() << '\n';
        file << "Loyalty discount: " << order.getLoyaltyDiscount() << '\n';
        file << "Amount due: " << order.calculatePayableTotal() << '\n';
        file << "----------------------------------------\n";
    }

    file.flush();
    if (!file)
        throw InvalidOperationException("Error while writing report file");
    file.close();
    if (file.fail())
        throw InvalidOperationException("Error while closing report file");
}
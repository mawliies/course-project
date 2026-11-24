
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdexcept>
#include <string>

#include "restaurantManager.h"
#include "fileManager.h"
#include "dish.h"
#include "drink.h"
#include "client.h"
#include "employee.h"

namespace {
    int testsPassed = 0;
    int testsTotal = 0;

    void testCheck(bool condition, const std::string& testName) {
        ++testsTotal;

        if (!condition)
            throw std::runtime_error("TEST FAILED: " + testName);

        ++testsPassed;
        std::cout << "[OK] " << testName << '\n';
    }

    bool almostEqual(float first, float second) {
        return std::fabs(first - second) < 0.001f;
    }
}

int main() {
    try {
        const std::string databasePath = "restaurant_database.txt";

        FileManager fileManager(
            databasePath,
            "restaurant.log",
            "restaurant_report.txt"
        );

        std::cout << "===== PREPARING SOURCE DATA =====\n";

        RestaurantManager sourceManager;

        // 1. INGREDIENTS

        sourceManager.addIngredient(
            Ingredient(
                "Flour",
                NutritionValue(364, 10, 1, 76),
                MeasureUnit::gram
            )
        );

        sourceManager.addIngredient(
            Ingredient(
                "Sugar",
                NutritionValue(387, 0, 0, 100),
                MeasureUnit::gram
            )
        );

        // Добавляем и удаляем ингредиент,
        // чтобы проверить восстановление nextId.

        sourceManager.addIngredient(
            Ingredient(
                "Milk",
                NutritionValue(60, 3, 3, 5),
                MeasureUnit::milliliter
            )
        );

        sourceManager.removeIngredient("Milk");

        testCheck(sourceManager.getCatalog().getAll().size() == 2,
            "Source catalog contains two ingredients");

        testCheck(sourceManager.getCatalog().getNextId() == 4,
            "Source next ingredient ID is 4");

        // 2. RECIPE

        Recipe pizzaRecipe(2);

        pizzaRecipe.addIngredient(
            200, "Flour", sourceManager.getCatalog()
        );

        pizzaRecipe.addIngredient(
            50, "Sugar", sourceManager.getCatalog()
        );

        // 3. MAIN MENU

        sourceManager.addToMainMenu(
            new Dish(
                "Homemade Pizza",
                15.5f,
                "Main course",
                pizzaRecipe,
                30,
                450
            )
        );

        // 4. SEASONAL MENU

        Recipe drinkRecipe(1);

        sourceManager.addToSeasonalMenu(
            new Drink(
                "Fresh Lemonade",
                5.0f,
                "Drink",
                drinkRecipe,
                500,
                true
            )
        );

        sourceManager.setMainMenuUpdateDate(24, 11, 2026);

        sourceManager.setSeasonalMenuPeriod(
            1, 6, 2026,
            31, 8, 2026
        );

        // 5. PEOPLE

        sourceManager.addPerson(
            new Client(
                "Ivan",
                "Ivanov",
                "Ivanovich",
                "+375291111111",
                150
            )
        );

        sourceManager.addPerson(
            new Employee(
                "Petr",
                "Petrov",
                "Petrovich",
                "+375331234567",
                "Morning",
                "Cook"
            )
        );

        // 6. ORDER

        Order sourceOrder("+375291111111", OrderStatus::New);

        // Историческая цена отличается от текущей цены блюда.

        OrderItem historicalItem;
        historicalItem.name = "Homemade Pizza";
        historicalItem.amount = 2;
        historicalItem.priceAtOrder = 14.25f;

        sourceOrder.restore(historicalItem);

        sourceManager.addOrder(sourceOrder);

        testCheck(almostEqual(sourceOrder.calculateTotal(), 28.5f),
            "Source order total uses historical price");

        // 7. SAVE DATABASE

        std::cout << "\n===== SAVING DATABASE =====\n";

        fileManager.saveDatabase(sourceManager);

        std::cout << "Database saved to: "
            << databasePath << "\n";

        // 8. DISPLAY DATABASE FILE

        std::ifstream databaseFile(databasePath);

        if (!databaseFile.is_open())
            throw std::runtime_error(
                "Cannot open saved database file for display"
            );

        std::cout << "\n===== DATABASE CONTENT =====\n";

        std::string line;

        while (std::getline(databaseFile, line))
            std::cout << line << '\n';

        if (databaseFile.bad())
            throw std::runtime_error(
                "Error while displaying database file"
            );

        databaseFile.close();

        std::cout << "===== END OF DATABASE =====\n";

        // 9. LOAD DATABASE INTO A NEW MANAGER

        std::cout << "\n===== LOADING DATABASE =====\n";

        RestaurantManager loadedManager;

        fileManager.loadDatabase(loadedManager);

        std::cout << "Database loaded successfully.\n\n";

        // 10. TEST INGREDIENTS

        const auto& loadedIngredients =
            loadedManager.getCatalog().getAll();

        testCheck(loadedIngredients.size() == 2,
            "Loaded ingredient count");

        testCheck(loadedManager.getCatalog().getNextId() == 4,
            "Loaded next ingredient ID");

        const Ingredient* loadedFlour =
            loadedManager.findIngredient("Flour");

        testCheck(loadedFlour != nullptr,
            "Flour exists after loading");

        if (loadedFlour != nullptr) {
            testCheck(loadedFlour->getId() == 1,
                "Flour ID restored");

            testCheck(loadedFlour->getUnit() == MeasureUnit::gram,
                "Flour measure unit restored");

            testCheck(almostEqual(
                loadedFlour->getNutrition().getCalories(), 364),
                "Flour calories restored");

            testCheck(almostEqual(
                loadedFlour->getNutrition().getProteins(), 10),
                "Flour proteins restored");
        }

        const Ingredient* loadedSugar =
            loadedManager.findIngredient("Sugar");

        testCheck(loadedSugar != nullptr,
            "Sugar exists after loading");

        if (loadedSugar != nullptr) {
            testCheck(loadedSugar->getId() == 2,
                "Sugar ID restored");

            testCheck(almostEqual(
                loadedSugar->getNutrition().getCarbs(), 100),
                "Sugar carbohydrates restored");
        }

        testCheck(
            loadedManager.findIngredient("Milk") == nullptr,
            "Removed ingredient is not restored"
        );

        // 11. TEST MAIN MENU AND DISH

        const auto loadedMainItems =
            loadedManager.getMainMenu().getItems();

        testCheck(loadedMainItems.size() == 1,
            "Main menu item count");

        const MenuObject* loadedPizza =
            loadedManager.findInMainMenu("Homemade Pizza");

        testCheck(loadedPizza != nullptr,
            "Pizza exists in main menu");

        const Dish* loadedDish =
            dynamic_cast<const Dish*>(loadedPizza);

        testCheck(loadedDish != nullptr,
            "Pizza restored as Dish");

        if (loadedDish != nullptr) {
            testCheck(almostEqual(loadedDish->getPrice(), 15.5f),
                "Dish price restored");

            testCheck(loadedDish->getCategory() == "Main course",
                "Dish category restored");

            testCheck(loadedDish->getTime() == 30,
                "Dish cooking time restored");

            testCheck(loadedDish->getWeight() == 450,
                "Dish weight restored");

            testCheck(loadedDish->getRecipe().getPortions() == 2,
                "Dish recipe portions restored");

            const auto& recipeIngredients =
                loadedDish->getRecipe().getIngredients();

            testCheck(recipeIngredients.size() == 2,
                "Dish recipe ingredient count restored");

            if (recipeIngredients.size() == 2) {
                testCheck(recipeIngredients[0].ingredientId == 1,
                    "First recipe ingredient ID restored");

                testCheck(almostEqual(recipeIngredients[0].amount, 200),
                    "First recipe ingredient amount restored");

                testCheck(recipeIngredients[1].ingredientId == 2,
                    "Second recipe ingredient ID restored");

                testCheck(almostEqual(recipeIngredients[1].amount, 50),
                    "Second recipe ingredient amount restored");
            }
        }

        testCheck(
            loadedManager.getMainMenu().getDay() == 24 &&
            loadedManager.getMainMenu().getMonth() == 11 &&
            loadedManager.getMainMenu().getYear() == 2026,
            "Main menu update date restored"
        );

        // 12. TEST SEASONAL MENU AND DRINK

        const auto loadedSeasonalItems =
            loadedManager.getSeasonalMenu().getItems();

        testCheck(loadedSeasonalItems.size() == 1,
            "Seasonal menu item count");

        const MenuObject* loadedLemonade =
            loadedManager.findInSeasonalMenu("Fresh Lemonade");

        testCheck(loadedLemonade != nullptr,
            "Lemonade exists in seasonal menu");

        const Drink* loadedDrink =
            dynamic_cast<const Drink*>(loadedLemonade);

        testCheck(loadedDrink != nullptr,
            "Lemonade restored as Drink");

        if (loadedDrink != nullptr) {
            testCheck(almostEqual(loadedDrink->getPrice(), 5.0f),
                "Drink price restored");

            testCheck(loadedDrink->getCategory() == "Drink",
                "Drink category restored");

            testCheck(loadedDrink->getVolume() == 500,
                "Drink volume restored");

            testCheck(loadedDrink->getIsCold(),
                "Drink cold flag restored");

            testCheck(loadedDrink->getRecipe().getPortions() == 1,
                "Drink recipe portions restored");

            testCheck(
                loadedDrink->getRecipe().getIngredients().empty(),
                "Empty drink recipe restored"
            );
        }

        testCheck(
            loadedManager.getSeasonalMenu().getDayBegin() == 1 &&
            loadedManager.getSeasonalMenu().getMonthBegin() == 6 &&
            loadedManager.getSeasonalMenu().getYearBegin() == 2026 &&
            loadedManager.getSeasonalMenu().getDayEnd() == 31 &&
            loadedManager.getSeasonalMenu().getMonthEnd() == 8 &&
            loadedManager.getSeasonalMenu().getYearEnd() == 2026,
            "Seasonal menu period restored"
        );

        // 13. TEST PEOPLE

        const auto& loadedPeople = loadedManager.getPeople();

        testCheck(loadedPeople.getSize() == 2,
            "People count restored");

        const Client* loadedClient = nullptr;
        const Employee* loadedEmployee = nullptr;

        if (loadedPeople.getSize() >= 2) {
            loadedClient =
                dynamic_cast<const Client*>(loadedPeople.get(0));

            loadedEmployee =
                dynamic_cast<const Employee*>(loadedPeople.get(1));
        }

        testCheck(loadedClient != nullptr,
            "First person restored as Client");

        if (loadedClient != nullptr) {
            testCheck(loadedClient->getFirstName() == "Ivan",
                "Client first name restored");

            testCheck(loadedClient->getLastName() == "Ivanov",
                "Client last name restored");

            testCheck(loadedClient->getMiddlelName() == "Ivanovich",
                "Client middle name restored");

            testCheck(loadedClient->getPhoneNumber() == "+375291111111",
                "Client phone restored");

            testCheck(loadedClient->getBonus() == 150,
                "Client bonus restored");
        }

        testCheck(loadedEmployee != nullptr,
            "Second person restored as Employee");

        if (loadedEmployee != nullptr) {
            testCheck(loadedEmployee->getFirstName() == "Petr",
                "Employee first name restored");

            testCheck(loadedEmployee->getPhoneNumber() == "+375331234567",
                "Employee phone restored");

            testCheck(loadedEmployee->getShift() == "Morning",
                "Employee shift restored");

            testCheck(loadedEmployee->getPosition() == "Cook",
                "Employee position restored");
        }

        // 14. TEST ORDERS

        testCheck(loadedManager.getOrdersCount() == 1,
            "Order count restored");

        testCheck(loadedManager.getOrders().size() == 1,
            "Orders vector size restored");

        if (loadedManager.getOrders().size() == 1) {
            const Order& loadedOrder =
                loadedManager.getOrders()[0];

            testCheck(
                loadedOrder.getClientPhoneNumber() == "+375291111111",
                "Order client phone restored"
            );

            testCheck(loadedOrder.getStatus() == OrderStatus::New,
                "Order status restored");

            testCheck(loadedOrder.getItems().size() == 1,
                "Order item count restored");

            if (loadedOrder.getItems().size() == 1) {
                const OrderItem& loadedItem =
                    loadedOrder.getItems()[0];

                testCheck(loadedItem.name == "Homemade Pizza",
                    "Order item name restored");

                testCheck(loadedItem.amount == 2,
                    "Order item amount restored");

                testCheck(almostEqual(loadedItem.priceAtOrder, 14.25f),
                    "Historical item price restored");
            }

            testCheck(almostEqual(loadedOrder.calculateTotal(), 28.5f),
                "Order total restored using historical price");
        }

        // 15. TEST NEXT INGREDIENT ID AFTER RESTORATION

        loadedManager.addIngredient(
            Ingredient(
                "Salt",
                NutritionValue(0, 0, 0, 0),
                MeasureUnit::gram
            )
        );

        const Ingredient* loadedSalt =
            loadedManager.findIngredient("Salt");

        testCheck(loadedSalt != nullptr,
            "New ingredient can be added after loading");

        if (loadedSalt != nullptr) {
            testCheck(loadedSalt->getId() == 4,
                "New ingredient receives the restored next ID");
        }

        // FINAL RESULT

        std::cout << "\n===== DATABASE TEST RESULT =====\n";

        std::cout << "Passed: "
            << testsPassed << " / " << testsTotal << '\n';

        std::cout << "All database save/load checks passed.\n";
    }
    catch (const std::exception& exception) {
        std::cout << "\n" << exception.what() << '\n';

        std::cout << "Tests passed before failure: "
            << testsPassed << " / " << testsTotal << '\n';

        return 1;
    }

    return 0;
}

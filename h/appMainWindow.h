#pragma once

#include <QMainWindow>
#include <QString>
#include <vector>
#include <string>

class RestaurantManager;
class FileManager;
class MenuObject;
class QStackedWidget;
class QLineEdit;
class QPushButton;
class QComboBox;
class QSpinBox;
class QTableWidget;
class QVBoxLayout;
class QLabel;
class QWidget;
class QCloseEvent;

class AppMainWindow final : public QMainWindow {
public:
    AppMainWindow(RestaurantManager& manager,
                  FileManager& fileManager,
                  const QString& dataDirectory,
                  QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    struct CartEntry {
        const MenuObject* item = nullptr;
        int quantity = 0;
    };

    RestaurantManager& manager_;
    FileManager& fileManager_;
    QString dataDirectory_;

    QStackedWidget* pages_ = nullptr;
    QWidget* welcomePage_ = nullptr;
    QWidget* orderPage_ = nullptr;
    QWidget* managerPage_ = nullptr;
    QWidget* customerPage_ = nullptr;
    QString currentCustomerPhone_;

    QLineEdit* firstNameEdit_ = nullptr;
    QLineEdit* lastNameEdit_ = nullptr;
    QLineEdit* middleNameEdit_ = nullptr;
    QLineEdit* phoneEdit_ = nullptr;
    QSpinBox* guestCountSpin_ = nullptr;
    QComboBox* tableCombo_ = nullptr;
    QVBoxLayout* mainMenuCardsLayout_ = nullptr;
    QVBoxLayout* seasonalMenuCardsLayout_ = nullptr;
    QLabel* seasonalMenuStatusLabel_ = nullptr;
    QTableWidget* cartTable_ = nullptr;
    QLabel* totalLabel_ = nullptr; // basket subtotal
    QLabel* loyaltyDiscountLabel_ = nullptr;
    QLabel* payableTotalLabel_ = nullptr;
    QLabel* bonusAvailabilityLabel_ = nullptr;
    QSpinBox* redeemPointsSpin_ = nullptr;
    QLabel* tableHintLabel_ = nullptr;
    std::vector<CartEntry> cart_;

    QTableWidget* managerOrdersTable_ = nullptr;
    QPushButton* startPreparationButton_ = nullptr;
    QPushButton* markReadyButton_ = nullptr;
    QPushButton* markServedButton_ = nullptr;
    QPushButton* cancelOrderButton_ = nullptr;
    QTableWidget* managerTablesTable_ = nullptr;
    QTableWidget* managerMenuTable_ = nullptr; // Main Menu
    QTableWidget* managerSeasonalMenuTable_ = nullptr;
    QTableWidget* managerIngredientsTable_ = nullptr;
    QTableWidget* managerPeopleTable_ = nullptr;
    QLabel* overviewOrdersLabel_ = nullptr;
    QLabel* overviewTablesLabel_ = nullptr;
    QLabel* overviewMenuLabel_ = nullptr;
    QLabel* customerNameLabel_ = nullptr;
    QLabel* customerBonusLabel_ = nullptr;
    QTableWidget* customerOrdersTable_ = nullptr;
    QWidget* createWelcomePage();
    QWidget* createOrderPage();
    QWidget* createManagerPage();

    void openOrderPage();
    void openManagerLogin();
    void openCustomerAccount();
    QWidget* createCustomerPage();
    void refreshCustomerProfile();
    void applyCustomerToOrderForm();
    void openNutritionAdvisor();
    void showAboutDialog();
    void refreshTableOptions();
    void rebuildMenuCards();
    void addToCart(const MenuObject* item, int quantity);
    void refreshCart();
    void handleCheckout();

    void refreshManagerDashboard();
    void refreshManagerOrders();
    void refreshManagerTables();
    void refreshManagerMenu();
    void refreshManagerIngredients();
    void refreshManagerPeople();
    void changeSelectedOrderStatus(int statusValue);
    void updateOrderActionButtons();
    void addDiningTable();
    void removeSelectedDiningTable();

    bool verifyManagerCredentials(const QString& username,
                                  const QString& password) const;
    bool persist(const QString& action, bool generateReport = false);
    void returnToWelcome();
};

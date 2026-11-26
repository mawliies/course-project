#include "appMainWindow.h"

#include "restaurantManager.h"
#include "fileManager.h"
#include "menuObject.h"
#include "dish.h"
#include "drink.h"
#include "client.h"
#include "employee.h"
#include "ingredient.h"
#include "nutritionAdvisor.h"
#include "nutritionRequest.h"
#include "orderBuilder.h"
#include "nutritionValue.h"
#include "diningTable.h"
#include "AllExceptions.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QByteArray>
#include <QCloseEvent>
#include <QComboBox>
#include <QCryptographicHash>
#include <QDialog>
#include <QDir>
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QColor>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFrame>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QRegularExpression>
#include <QStringList>
#include <QRegularExpressionValidator>
#include <QScrollArea>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QListWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QSaveFile>
#include <QSignalBlocker>
#include <QUuid>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <exception>
#include <cmath>

namespace {
QString q(const std::string& value) {
    return QString::fromUtf8(value.data(), static_cast<int>(value.size()));
}

QString money(float value) {
    return QString::number(value, 'f', 2) + " BYN";
}

QString orderStatusText(OrderStatus status) {
    switch (status) {
    case OrderStatus::Draft: return "Draft";
    case OrderStatus::New: return "New";
    case OrderStatus::Ready: return "Ready";
    case OrderStatus::Served: return "Served";
    case OrderStatus::Cancelled: return "Cancelled";
    case OrderStatus::Preparing: return "Preparing";
    }
    return "Unknown";
}

QFrame* makePanel(const QString& objectName = QStringLiteral("Panel")) {
    auto* panel = new QFrame;
    panel->setObjectName(objectName);
    panel->setFrameShape(QFrame::NoFrame);
    return panel;
}

QLabel* heading(const QString& text) {
    auto* label = new QLabel(text);
    label->setObjectName("SectionTitle");
    return label;
}

QPushButton* actionButton(const QString& text, bool primary = false) {
    auto* button = new QPushButton(text);
    button->setObjectName(primary ? "PrimaryButton" : "SecondaryButton");
    button->setCursor(Qt::PointingHandCursor);
    button->setMinimumHeight(42);
    return button;
}

void configureTable(QTableWidget* table) {
    table->setAlternatingRowColors(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setShowGrid(false);
}

QString ingredientUnit(MeasureUnit unit) {
    return unit == MeasureUnit::gram ? "g" : "ml";
}

QString customerPasswordHash(const QString& salt, const QString& password) {
    const QByteArray source = (salt + "|" + password).toUtf8();
    return QString::fromLatin1(QCryptographicHash::hash(source, QCryptographicHash::Sha256).toHex());
}

bool readCustomerAccounts(const QString& path, QJsonObject& accounts, QString* errorMessage = nullptr) {
    accounts = QJsonObject();
    if (!QFileInfo::exists(path))
        return true;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        if (errorMessage) *errorMessage = QString("Invalid customer account file: %1").arg(parseError.errorString());
        return false;
    }
    accounts = document.object();
    return true;
}

bool writeCustomerAccounts(const QString& path, const QJsonObject& accounts, QString* errorMessage = nullptr) {
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    const QByteArray data = QJsonDocument(accounts).toJson(QJsonDocument::Indented);
    if (file.write(data) != data.size()) {
        if (errorMessage) *errorMessage = file.errorString();
        file.cancelWriting();
        return false;
    }
    if (!file.commit()) {
        if (errorMessage) *errorMessage = file.errorString();
        return false;
    }
    return true;
}
}
AppMainWindow::AppMainWindow(RestaurantManager& manager,
                             FileManager& fileManager,
                             const QString& dataDirectory,
                             QWidget* parent)
    : QMainWindow(parent), manager_(manager), fileManager_(fileManager), dataDirectory_(dataDirectory) {
    setWindowTitle("ARCANUM — Beyond the Stars");
    setMinimumSize(1180, 760);
    resize(1440, 900);
    setWindowIcon(QIcon(":/images/arcanum_emblem.png"));

    pages_ = new QStackedWidget(this);
    pages_->setObjectName("AppRoot");
    setCentralWidget(pages_);

    welcomePage_ = createWelcomePage();
    pages_->addWidget(welcomePage_);
    pages_->setCurrentWidget(welcomePage_);
}

QWidget* AppMainWindow::createWelcomePage() {
    auto* page = new QWidget;
    page->setObjectName("WelcomePage");

    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(54, 30, 54, 28);
    root->setSpacing(20);

    auto* topBar = new QHBoxLayout;
    auto* emblem = new QLabel;
    emblem->setObjectName("HeaderEmblem");
    emblem->setPixmap(QPixmap(":/images/arcanum_emblem.png").scaled(58, 58, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    topBar->addWidget(emblem);

    auto* brandBlock = new QVBoxLayout;
    auto* brand = new QLabel("ARCANUM");
    brand->setObjectName("BrandTitle");
    auto* brandCaption = new QLabel("CELESTIAL DINING HOUSE");
    brandCaption->setObjectName("BrandCaption");
    brandBlock->addWidget(brand);
    brandBlock->addWidget(brandCaption);
    topBar->addLayout(brandBlock);
    topBar->addStretch();

    auto* accountButton = actionButton("CUSTOMER ACCOUNT");
    connect(accountButton, &QPushButton::clicked, this, [this]() { openCustomerAccount(); });
    topBar->addWidget(accountButton);

    auto* headerAbout = actionButton("ABOUT ARCANUM");
    connect(headerAbout, &QPushButton::clicked, this, [this]() { showAboutDialog(); });
    topBar->addWidget(headerAbout);
    root->addLayout(topBar);

    auto* hero = makePanel("HeroPanel");
    hero->setMinimumHeight(570);
    auto* heroLayout = new QHBoxLayout(hero);
    heroLayout->setContentsMargins(54, 48, 42, 42);
    heroLayout->setSpacing(28);

    auto* copy = new QVBoxLayout;
    copy->setSpacing(18);
    auto* eyebrow = new QLabel("A PLACE BETWEEN WORLDS");
    eyebrow->setObjectName("Eyebrow");
    copy->addWidget(eyebrow);

    auto* title = new QLabel("ARCANUM");
    title->setObjectName("HeroTitle");
    copy->addWidget(title);

    auto* subtitle = new QLabel("A constellation of unforgettable flavours");
    subtitle->setObjectName("HeroSubtitle");
    subtitle->setWordWrap(true);
    copy->addWidget(subtitle);

    auto* description = new QLabel(
        "Welcome to a place where refined cuisine meets the beauty of the night sky.\n"
        "Explore the menu, choose your table, and make the evening extraordinary.");
    description->setObjectName("BodyText");
    description->setWordWrap(true);
    description->setMaximumWidth(620);
    copy->addWidget(description);
    copy->addSpacing(14);

    auto* orderButton = actionButton("START YOUR ORDER", true);
    orderButton->setMinimumWidth(290);
    orderButton->setMinimumHeight(54);
    connect(orderButton, &QPushButton::clicked, this, [this]() { openOrderPage(); });
    copy->addWidget(orderButton, 0, Qt::AlignLeft);

    auto* managerButton = actionButton("MANAGER SIGN IN");
    managerButton->setMinimumWidth(290);
    managerButton->setMinimumHeight(50);
    connect(managerButton, &QPushButton::clicked, this, [this]() { openManagerLogin(); });
    copy->addWidget(managerButton, 0, Qt::AlignLeft);
    copy->addStretch();
    heroLayout->addLayout(copy, 3);

    auto* emblemColumn = new QVBoxLayout;
    emblemColumn->addStretch();
    auto* largeEmblem = new QLabel;
    largeEmblem->setAlignment(Qt::AlignCenter);
    largeEmblem->setPixmap(QPixmap(":/images/arcanum_emblem.png").scaled(330, 330, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    emblemColumn->addWidget(largeEmblem, 0, Qt::AlignCenter);
    auto* motto = new QLabel("✦  PER ASPERA AD ASTRA  ✦");
    motto->setObjectName("Motto");
    motto->setAlignment(Qt::AlignCenter);
    emblemColumn->addWidget(motto);
    emblemColumn->addStretch();
    heroLayout->addLayout(emblemColumn, 2);

    root->addWidget(hero, 1);

    auto* footer = new QHBoxLayout;
    auto* footerText = new QLabel("ARCANUM  •  A DINING HOUSE BEYOND THE STARS");
    footerText->setObjectName("FooterText");
    footer->addWidget(footerText);
    footer->addStretch();
    auto* version = new QLabel("EST. 2026");
    version->setObjectName("FooterText");
    footer->addWidget(version);
    root->addLayout(footer);
    return page;
}

void AppMainWindow::showAboutDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("About ARCANUM");
    dialog.setMinimumWidth(620);
    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(30, 28, 30, 26);

    auto* logo = new QLabel;
    logo->setAlignment(Qt::AlignCenter);
    logo->setPixmap(QPixmap(":/images/arcanum_emblem.png").scaled(112, 112, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    layout->addWidget(logo);
    auto* title = heading("ARCANUM — The Art of Experience");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    auto* text = new QLabel(
        "ARCANUM is a premium dining house inspired by the night sky, "
        "the symbolism of the Sun and Moon, and antique celestial charts.\n\n"
        "Our concept is built around attention to detail: a thoughtful menu, a calm atmosphere, "
        "refined presentation, and signature non-alcoholic drinks. Every menu item has "
        "a name inspired by constellations and celestial phenomena.");
    text->setObjectName("BodyText");
    text->setWordWrap(true);
    layout->addWidget(text);

    auto* closeButton = actionButton("CLOSE", true);
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(closeButton);
    dialog.exec();
}

void AppMainWindow::openOrderPage() {
    if (!orderPage_) {
        orderPage_ = createOrderPage();
        pages_->addWidget(orderPage_);
    }
    refreshTableOptions();
    rebuildMenuCards();
    refreshCart();
    applyCustomerToOrderForm();
    pages_->setCurrentWidget(orderPage_);
}

QWidget* AppMainWindow::createOrderPage() {
    auto* page = new QWidget;
    page->setObjectName("AppPage");
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(26, 22, 26, 22);
    root->setSpacing(16);

    auto* header = new QHBoxLayout;
    auto* backButton = actionButton("←  HOME");
    connect(backButton, &QPushButton::clicked, this, [this]() { returnToWelcome(); });
    header->addWidget(backButton);
    header->addStretch();
    auto* title = heading("PLACE YOUR ORDER");
    header->addWidget(title);
    header->addStretch();
    auto* accountButton = actionButton("MY ACCOUNT");
    connect(accountButton, &QPushButton::clicked, this, [this]() { openCustomerAccount(); });
    header->addWidget(accountButton);
    auto* emblem = new QLabel("☾  ✦  ☼");
    emblem->setObjectName("DecorativeMark");
    header->addWidget(emblem);
    root->addLayout(header);

    auto* columns = new QHBoxLayout;
    columns->setSpacing(16);

    // Customer and seating panel.
    auto* customerPanel = makePanel();
    customerPanel->setMinimumWidth(285);
    customerPanel->setMaximumWidth(335);
    auto* customerLayout = new QVBoxLayout(customerPanel);
    customerLayout->setContentsMargins(20, 20, 20, 20);
    customerLayout->setSpacing(12);
    customerLayout->addWidget(heading("YOUR VISIT"));

    firstNameEdit_ = new QLineEdit;
    firstNameEdit_->setPlaceholderText("First name");
    lastNameEdit_ = new QLineEdit;
    lastNameEdit_->setPlaceholderText("Last name");
    middleNameEdit_ = new QLineEdit;
    middleNameEdit_->setPlaceholderText("Middle name (optional)");
    phoneEdit_ = new QLineEdit;
    phoneEdit_->setPlaceholderText("+375291234567");
    phoneEdit_->setMaxLength(13);
    phoneEdit_->setValidator(new QRegularExpressionValidator(QRegularExpression("\\+375[0-9]{0,9}"), phoneEdit_));

    customerLayout->addWidget(new QLabel("First name"));
    customerLayout->addWidget(firstNameEdit_);
    customerLayout->addWidget(new QLabel("Last name"));
    customerLayout->addWidget(lastNameEdit_);
    customerLayout->addWidget(new QLabel("Middle name"));
    customerLayout->addWidget(middleNameEdit_);
    customerLayout->addWidget(new QLabel("Phone number"));
    customerLayout->addWidget(phoneEdit_);

    auto* line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setObjectName("GoldDivider");
    customerLayout->addWidget(line);
    customerLayout->addWidget(heading("TABLE RESERVATION"));
    customerLayout->addWidget(new QLabel("Number of guests"));
    guestCountSpin_ = new QSpinBox;
    guestCountSpin_->setRange(1, 8);
    guestCountSpin_->setValue(2);
    customerLayout->addWidget(guestCountSpin_);
    customerLayout->addWidget(new QLabel("Available table"));
    tableCombo_ = new QComboBox;
    customerLayout->addWidget(tableCombo_);
    tableHintLabel_ = new QLabel;
    tableHintLabel_->setObjectName("HintText");
    tableHintLabel_->setWordWrap(true);
    customerLayout->addWidget(tableHintLabel_);
    customerLayout->addStretch();
    auto* phoneHint = new QLabel("Enter your phone number in the format +375XXXXXXXXX.");
    phoneHint->setObjectName("HintText");
    phoneHint->setWordWrap(true);
    customerLayout->addWidget(phoneHint);

    connect(guestCountSpin_, qOverload<int>(&QSpinBox::valueChanged), this,
            [this](int) { refreshTableOptions(); });
    columns->addWidget(customerPanel);

    // Menu catalog panel.
    auto* catalogPanel = makePanel();
    auto* catalogLayout = new QVBoxLayout(catalogPanel);
    catalogLayout->setContentsMargins(20, 20, 20, 20);
    catalogLayout->setSpacing(12);
    auto* catalogHeader = new QHBoxLayout;
    catalogHeader->addWidget(heading("RESTAURANT MENU"));
    catalogHeader->addStretch();
    auto* catalogCaption = new QLabel("✦  ARCANUM SELECTION");
    catalogCaption->setObjectName("HintText");
    catalogHeader->addWidget(catalogCaption);
    auto* nutritionButton = actionButton("FIND MY NUTRITION MATCH", true);
    nutritionButton->setMinimumHeight(38);
    connect(nutritionButton, &QPushButton::clicked, this, [this]() { openNutritionAdvisor(); });
    catalogHeader->addWidget(nutritionButton);
    catalogLayout->addLayout(catalogHeader);

    auto* menuTabs = new QTabWidget;
    auto createMenuPage = [](QVBoxLayout** cardsLayout) {
        auto* page = new QWidget;
        auto* pageLayout = new QVBoxLayout(page);
        pageLayout->setContentsMargins(6, 6, 6, 6);
        pageLayout->setSpacing(8);
        auto* scrollArea = new QScrollArea;
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        auto* content = new QWidget;
        *cardsLayout = new QVBoxLayout(content);
        (*cardsLayout)->setContentsMargins(2, 2, 6, 2);
        (*cardsLayout)->setSpacing(10);
        scrollArea->setWidget(content);
        pageLayout->addWidget(scrollArea, 1);
        return page;
    };
    auto* mainMenuPage = createMenuPage(&mainMenuCardsLayout_);
    auto* seasonalMenuPage = new QWidget;
    auto* seasonalPageLayout = new QVBoxLayout(seasonalMenuPage);
    seasonalPageLayout->setContentsMargins(6, 6, 6, 6);
    seasonalPageLayout->setSpacing(8);
    seasonalMenuStatusLabel_ = new QLabel;
    seasonalMenuStatusLabel_->setObjectName("HintText");
    seasonalMenuStatusLabel_->setWordWrap(true);
    seasonalPageLayout->addWidget(seasonalMenuStatusLabel_);
    auto* seasonalScroll = new QScrollArea;
    seasonalScroll->setWidgetResizable(true);
    seasonalScroll->setFrameShape(QFrame::NoFrame);
    auto* seasonalContent = new QWidget;
    seasonalMenuCardsLayout_ = new QVBoxLayout(seasonalContent);
    seasonalMenuCardsLayout_->setContentsMargins(2, 2, 6, 2);
    seasonalMenuCardsLayout_->setSpacing(10);
    seasonalScroll->setWidget(seasonalContent);
    seasonalPageLayout->addWidget(seasonalScroll, 1);

    menuTabs->addTab(mainMenuPage, "Main Menu");
    menuTabs->addTab(seasonalMenuPage, "Seasonal Menu");
    catalogLayout->addWidget(menuTabs, 1);
    columns->addWidget(catalogPanel, 1);

    // Basket panel.
    auto* basketPanel = makePanel();
    basketPanel->setMinimumWidth(285);
    basketPanel->setMaximumWidth(340);
    auto* basketLayout = new QVBoxLayout(basketPanel);
    basketLayout->setContentsMargins(18, 20, 18, 20);
    basketLayout->setSpacing(12);
    basketLayout->addWidget(heading("YOUR ORDER"));
    cartTable_ = new QTableWidget(0, 4);
    cartTable_->setHorizontalHeaderLabels({"Item", "Qty", "Subtotal", ""});
    configureTable(cartTable_);
    cartTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    cartTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    cartTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    cartTable_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    cartTable_->setColumnWidth(3, 36);
    basketLayout->addWidget(cartTable_, 1);

    auto* subtotalRow = new QHBoxLayout;
    subtotalRow->addWidget(new QLabel("SUBTOTAL"));
    subtotalRow->addStretch();
    totalLabel_ = new QLabel("0.00 BYN");
    totalLabel_->setObjectName("TotalLabel");
    subtotalRow->addWidget(totalLabel_);
    basketLayout->addLayout(subtotalRow);

    auto* redemptionTitle = heading("LOYALTY REDEMPTION");
    basketLayout->addWidget(redemptionTitle);
    bonusAvailabilityLabel_ = new QLabel("Sign in to redeem points. 100 points = 1 BYN.");
    bonusAvailabilityLabel_->setObjectName("HintText");
    bonusAvailabilityLabel_->setWordWrap(true);
    basketLayout->addWidget(bonusAvailabilityLabel_);
    auto* redemptionRow = new QHBoxLayout;
    redemptionRow->addWidget(new QLabel("Points to use"));
    redemptionRow->addStretch();
    redeemPointsSpin_ = new QSpinBox;
    redeemPointsSpin_->setRange(0, 0);
    redeemPointsSpin_->setMaximumWidth(110);
    redeemPointsSpin_->setEnabled(false);
    redemptionRow->addWidget(redeemPointsSpin_);
    basketLayout->addLayout(redemptionRow);

    auto* discountRow = new QHBoxLayout;
    discountRow->addWidget(new QLabel("LOYALTY DISCOUNT"));
    discountRow->addStretch();
    loyaltyDiscountLabel_ = new QLabel("0.00 BYN");
    discountRow->addWidget(loyaltyDiscountLabel_);
    basketLayout->addLayout(discountRow);

    auto* dueRow = new QHBoxLayout;
    dueRow->addWidget(new QLabel("AMOUNT DUE"));
    dueRow->addStretch();
    payableTotalLabel_ = new QLabel("0.00 BYN");
    payableTotalLabel_->setObjectName("TotalLabel");
    dueRow->addWidget(payableTotalLabel_);
    basketLayout->addLayout(dueRow);
    connect(redeemPointsSpin_, qOverload<int>(&QSpinBox::valueChanged), this,
            [this](int) { refreshCart(); });

    auto* checkoutButton = actionButton("CONFIRM ORDER", true);
    checkoutButton->setMinimumHeight(48);
    connect(checkoutButton, &QPushButton::clicked, this, [this]() { handleCheckout(); });
    basketLayout->addWidget(checkoutButton);
    auto* clearButton = actionButton("CLEAR BASKET");
    connect(clearButton, &QPushButton::clicked, this, [this]() { cart_.clear(); refreshCart(); });
    basketLayout->addWidget(clearButton);
    columns->addWidget(basketPanel);

    root->addLayout(columns, 1);
    return page;
}

void AppMainWindow::refreshTableOptions() {
    if (!tableCombo_ || !guestCountSpin_)
        return;

    const int previousNumber = tableCombo_->currentData().toInt();
    tableCombo_->clear();
    const auto available = manager_.getDiningRoom().getAvailableTables(guestCountSpin_->value());
    for (const DiningTable* table : available) {
        if (!table)
            continue;
        const QString label = QString("Table %1  •  %2 seats")
            .arg(table->getNumber()).arg(table->getCapacity());
        tableCombo_->addItem(label, table->getNumber());
    }

    const int previousIndex = tableCombo_->findData(previousNumber);
    if (previousIndex >= 0)
        tableCombo_->setCurrentIndex(previousIndex);

    const bool hasTables = tableCombo_->count() > 0;
    tableCombo_->setEnabled(hasTables);
    tableHintLabel_->setText(hasTables
        ? "Only available tables that fit your party size are shown."
        : "No available table can accommodate this party size.");
}

void AppMainWindow::rebuildMenuCards() {
    auto clearCards = [](QVBoxLayout* layout) {
        if (!layout)
            return;
        while (QLayoutItem* item = layout->takeAt(0)) {
            if (QWidget* widget = item->widget())
                widget->deleteLater();
            delete item;
        }
    };

    clearCards(mainMenuCardsLayout_);
    clearCards(seasonalMenuCardsLayout_);

    const auto mainItems = manager_.getMainMenu().getItems();
    const auto seasonalItems = manager_.getSeasonalMenu().getItems();
    const bool seasonalAvailable = manager_.getSeasonalMenu().isAvailable();

    if (seasonalMenuStatusLabel_) {
        const SeasonalMenu& seasonal = manager_.getSeasonalMenu();
        if (seasonalAvailable) {
            seasonalMenuStatusLabel_->setText("The seasonal selection is currently available.");
        }
        else if (seasonal.getDayBegin() > 0 && seasonal.getDayEnd() > 0) {
            seasonalMenuStatusLabel_->setText(
                QString("The seasonal selection is not currently active. It is scheduled for %1.%2.%3 – %4.%5.%6.")
                    .arg(seasonal.getDayBegin(), 2, 10, QChar('0'))
                    .arg(seasonal.getMonthBegin(), 2, 10, QChar('0'))
                    .arg(seasonal.getYearBegin())
                    .arg(seasonal.getDayEnd(), 2, 10, QChar('0'))
                    .arg(seasonal.getMonthEnd(), 2, 10, QChar('0'))
                    .arg(seasonal.getYearEnd()));
        }
        else {
            seasonalMenuStatusLabel_->setText("No seasonal menu period has been configured yet.");
        }
    }

    auto populate = [this](const std::vector<const MenuObject*>& items, QVBoxLayout* layout, bool canOrder) {
        if (!layout)
            return;
        if (items.empty()) {
            auto* empty = new QLabel("No items are currently listed in this menu.");
            empty->setObjectName("HintText");
            layout->addWidget(empty);
            layout->addStretch();
            return;
        }

        for (const MenuObject* object : items) {
            if (!object)
                continue;
            auto* card = makePanel("MenuCard");
            auto* row = new QHBoxLayout(card);
            row->setContentsMargins(14, 12, 14, 12);
            row->setSpacing(12);

            auto* info = new QVBoxLayout;
            auto* name = new QLabel(q(object->getTitle()));
            name->setObjectName("MenuItemTitle");
            name->setObjectName("MenuItemTitle");
            QStringList ingredientNames;

            const auto& recipeIngredients =
                object->getRecipe().getIngredients();

            for (const RecipeIngredient& recipeIngredient : recipeIngredients) {

                    const Ingredient* ingredient =
                        manager_.getCatalog().findById(recipeIngredient.ingredientId);

                    if (!ingredient)
                        continue;

                    const QString amount =
                        QString::number(recipeIngredient.amount, 'g', 4);

                    ingredientNames << QString("%1 (%2 %3)")
                        .arg(q(ingredient->getName()))
                        .arg(amount)
                        .arg(ingredientUnit(ingredient->getUnit()));
                
            }

            QLabel* ingredientsLabel = nullptr;

            if (!ingredientNames.isEmpty()) {
                ingredientsLabel = new QLabel(
                    "Ingredients: " + ingredientNames.join(" • ")
                );

                ingredientsLabel->setObjectName("HintText");
                ingredientsLabel->setWordWrap(true);
            }

            auto* category = new QLabel(q(object->getCategory()));
            category->setObjectName("MenuItemCategory");
            category->setObjectName("MenuItemCategory");
            QString detail;
            if (const auto* dish = dynamic_cast<const Dish*>(object))
                detail = QString("Preparation: %1 min  •  %2 g").arg(dish->getTime()).arg(dish->getWeight());
            else if (const auto* drink = dynamic_cast<const Drink*>(object))
                detail = QString("Volume: %1 ml  •  %2")
                    .arg(drink->getVolume()).arg(drink->getIsCold() ? "chilled" : "warm");
            auto* details = new QLabel(detail);
            details->setObjectName("HintText");
            info->addWidget(name);

            if (ingredientsLabel)
                info->addWidget(ingredientsLabel);

            info->addWidget(category);
            info->addWidget(details);
            row->addLayout(info, 1);

            auto* controls = new QVBoxLayout;
            auto* price = new QLabel(money(object->getPrice()));
            price->setObjectName("MenuItemPrice");
            price->setAlignment(Qt::AlignRight);
            auto* quantity = new QSpinBox;
            quantity->setRange(1, 20);
            quantity->setValue(1);
            quantity->setMaximumWidth(82);
            auto* addButton = actionButton("ADD", true);
            addButton->setMinimumWidth(112);
            addButton->setMinimumHeight(36);
            addButton->setEnabled(canOrder);
            if (!canOrder)
                addButton->setToolTip("This seasonal menu is outside its active period.");
            connect(addButton, &QPushButton::clicked, this,
                    [this, object, quantity]() { addToCart(object, quantity->value()); });
            controls->addWidget(price, 0, Qt::AlignRight);
            controls->addWidget(quantity, 0, Qt::AlignRight);
            controls->addWidget(addButton, 0, Qt::AlignRight);
            row->addLayout(controls);
            layout->addWidget(card);
        }
        layout->addStretch();
    };

    populate(mainItems, mainMenuCardsLayout_, true);
    populate(seasonalItems, seasonalMenuCardsLayout_, seasonalAvailable);
}

void AppMainWindow::addToCart(const MenuObject* item, int quantity) {
    if (!item || quantity <= 0)
        return;

    auto found = std::find_if(cart_.begin(), cart_.end(), [item](const CartEntry& entry) {
        return entry.item == item;
    });
    if (found == cart_.end())
        cart_.push_back({ item, quantity });
    else
        found->quantity += quantity;

    refreshCart();
}

void AppMainWindow::refreshCart() {
    if (!cartTable_ || !totalLabel_)
        return;

    cartTable_->setRowCount(0);
    float subtotal = 0.0f;
    for (int index = 0; index < static_cast<int>(cart_.size()); ++index) {
        const CartEntry& entry = cart_[static_cast<std::size_t>(index)];
        if (!entry.item)
            continue;
        const float lineTotal = entry.item->getPrice() * entry.quantity;
        subtotal += lineTotal;
        const int row = cartTable_->rowCount();
        cartTable_->insertRow(row);
        cartTable_->setItem(row, 0, new QTableWidgetItem(q(entry.item->getTitle())));
        cartTable_->setItem(row, 1, new QTableWidgetItem(QString::number(entry.quantity)));
        cartTable_->setItem(row, 2, new QTableWidgetItem(money(lineTotal)));
        auto* remove = new QPushButton("×");
        remove->setObjectName("SmallDangerButton");
        remove->setCursor(Qt::PointingHandCursor);
        connect(remove, &QPushButton::clicked, this, [this, index]() {
            if (index >= 0 && index < static_cast<int>(cart_.size()))
                cart_.erase(cart_.begin() + index);
            refreshCart();
        });
        cartTable_->setCellWidget(row, 3, remove);
    }
    totalLabel_->setText(money(subtotal));

    int availablePoints = 0;
    if (!currentCustomerPhone_.isEmpty()) {
        if (auto* client = dynamic_cast<Client*>(manager_.findPerson(currentCustomerPhone_.toStdString())))
            availablePoints = std::max(0, client->getBonus());
    }

    int maximumRedeemable = 0;
    if (subtotal > 0.0f)
        maximumRedeemable = static_cast<int>(std::floor(subtotal * LoyaltyPolicy::pointsPerBynDiscount + 0.0001f));
    maximumRedeemable = std::min(maximumRedeemable, availablePoints);

    int pointsToRedeem = 0;
    if (redeemPointsSpin_) {
        const QSignalBlocker blocker(redeemPointsSpin_);
        redeemPointsSpin_->setMaximum(maximumRedeemable);
        if (currentCustomerPhone_.isEmpty())
            redeemPointsSpin_->setValue(0);
        redeemPointsSpin_->setEnabled(maximumRedeemable > 0);
        pointsToRedeem = redeemPointsSpin_->value();
    }

    const float discount = static_cast<float>(pointsToRedeem) / static_cast<float>(LoyaltyPolicy::pointsPerBynDiscount);
    const float amountDue = std::max(0.0f, subtotal - discount);
    if (loyaltyDiscountLabel_)
        loyaltyDiscountLabel_->setText("− " + money(discount));
    if (payableTotalLabel_)
        payableTotalLabel_->setText(money(amountDue));
    if (bonusAvailabilityLabel_) {
        if (currentCustomerPhone_.isEmpty())
            bonusAvailabilityLabel_->setText("Sign in to redeem points. 100 points = 1 BYN.");
        else
            bonusAvailabilityLabel_->setText(QString("Available balance: %1 points. 100 points = 1 BYN.")
                .arg(availablePoints));
    }
}

void AppMainWindow::handleCheckout() {
    const QString firstName = firstNameEdit_->text().trimmed();
    const QString lastName = lastNameEdit_->text().trimmed();
    const QString middleName = middleNameEdit_->text().trimmed();
    const QString phone = phoneEdit_->text().trimmed();

    if (firstName.isEmpty() || lastName.isEmpty()) {
        QMessageBox::warning(this, "Missing details", "Enter the guest’s first and last name.");
        return;
    }
    if (!QRegularExpression("^\\+375[0-9]{9}$").match(phone).hasMatch()) {
        QMessageBox::warning(this, "Invalid phone number", "Enter a phone number in the format +375XXXXXXXXX.");
        return;
    }
    if (!currentCustomerPhone_.isEmpty() && phone != currentCustomerPhone_) {
        QMessageBox::warning(this, "Account phone mismatch",
            "You are signed in with a customer account. Place this order using the phone number linked to that account, or sign out first.");
        return;
    }
    if (cart_.empty()) {
        QMessageBox::warning(this, "Your basket is empty", "Add at least one menu item.");
        return;
    }
    if (!tableCombo_ || tableCombo_->currentIndex() < 0) {
        QMessageBox::warning(this, "No table selected", "Choose an available table that fits your party size.");
        return;
    }

    bool clientAdded = false;
    int newOrderIndex = -1;
    int redeemedPointsDeducted = 0;
    std::string redemptionPhone;
    try {
        Person* existingPerson = manager_.findPerson(phone.toStdString());
        if (!existingPerson) {
            auto* client = new Client(firstName.toStdString(), lastName.toStdString(),
                                      middleName.toStdString(), phone.toStdString(), 0);
            try {
                manager_.addPerson(client);
                clientAdded = true;
            }
            catch (...) {
                delete client;
                throw;
            }
        }
        else if (dynamic_cast<Client*>(existingPerson) == nullptr) {
            throw InvalidOperationException("This phone number belongs to an employee account.");
        }

        Order order(phone.toStdString(), OrderStatus::Draft, guestCountSpin_->value());
        for (const CartEntry& entry : cart_)
            if (entry.item)
                order.addItem(*entry.item, entry.quantity);

        const int pointsToRedeem = redeemPointsSpin_ ? redeemPointsSpin_->value() : 0;
        if (pointsToRedeem > 0) {
            auto* loyaltyClient = dynamic_cast<Client*>(manager_.findPerson(phone.toStdString()));
            if (!loyaltyClient || pointsToRedeem > loyaltyClient->getBonus())
                throw InvalidOperationException("The customer loyalty balance is insufficient for this redemption.");
            order.applyLoyaltyRedemption(pointsToRedeem, static_cast<float>(pointsToRedeem) / static_cast<float>(LoyaltyPolicy::pointsPerBynDiscount));
            loyaltyClient->setBonus(loyaltyClient->getBonus() - pointsToRedeem);
            redeemedPointsDeducted = pointsToRedeem;
            redemptionPhone = phone.toStdString();
        }

        manager_.addOrder(order);
        newOrderIndex = manager_.getOrdersCount() - 1;
        manager_.assignTableToOrder(newOrderIndex, tableCombo_->currentData().toInt());
        manager_.setOrderStatus(newOrderIndex, OrderStatus::New);

        if (!persist("New order created for " + phone, true))
            throw InvalidOperationException("The order was created, but the database could not be saved.");

        QMessageBox::information(this, "Order confirmed",
            QString("Your order has been placed!\n\nOrder number: %1\nTable: %2\nGuests: %3\nTotal: %4\n\nThank you for choosing ARCANUM.")
                .arg(newOrderIndex + 1)
                .arg(tableCombo_->currentData().toInt())
                .arg(guestCountSpin_->value())
                .arg(money(order.calculatePayableTotal())));

        cart_.clear();
        firstNameEdit_->clear();
        lastNameEdit_->clear();
        middleNameEdit_->clear();
        phoneEdit_->clear();
        if (redeemPointsSpin_)
            redeemPointsSpin_->setValue(0);
        guestCountSpin_->setValue(2);
        applyCustomerToOrderForm();
        refreshTableOptions();
        refreshCart();
        if (!currentCustomerPhone_.isEmpty())
            refreshCustomerProfile();
    }
    catch (const std::exception& error) {
        if (newOrderIndex >= 0) {
            try { manager_.removeOrder(newOrderIndex); } catch (...) {}
        }
        if (redeemedPointsDeducted > 0) {
            if (auto* client = dynamic_cast<Client*>(manager_.findPerson(redemptionPhone)))
                client->setBonus(client->getBonus() + redeemedPointsDeducted);
        }
        if (clientAdded) {
            try { manager_.removePerson(phone.toStdString()); } catch (...) {}
        }
        QMessageBox::critical(this, "Could not place order", QString::fromUtf8(error.what()));
    }
}

void AppMainWindow::openManagerLogin() {
    QDialog dialog(this);
    dialog.setWindowTitle("Manager Sign In");
    dialog.setMinimumWidth(420);
    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(26, 24, 26, 22);
    layout->addWidget(heading("MANAGEMENT ACCESS"));

    auto* form = new QFormLayout;
    auto* username = new QLineEdit;
    username->setPlaceholderText("Username");
    auto* password = new QLineEdit;
    password->setPlaceholderText("Password");
    password->setEchoMode(QLineEdit::Password);
    form->addRow("Username", username);
    form->addRow("Password", password);
    layout->addLayout(form);
    auto* note = new QLabel("This area is restricted to authorised restaurant managers.");
    note->setObjectName("HintText");
    note->setWordWrap(true);
    layout->addWidget(note);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText("SIGN IN");
    buttons->button(QDialogButtonBox::Cancel)->setText("CANCEL");
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted)
        return;

    if (!verifyManagerCredentials(username->text().trimmed(), password->text())) {
        QMessageBox::warning(this, "Access denied", "Incorrect username or password.");
        return;
    }

    if (!managerPage_) {
        managerPage_ = createManagerPage();
        pages_->addWidget(managerPage_);
    }
    refreshManagerDashboard();
    pages_->setCurrentWidget(managerPage_);
    persist("Manager signed in");
}

bool AppMainWindow::verifyManagerCredentials(const QString& username, const QString& password) const {
    QFile file(QDir(dataDirectory_).absoluteFilePath("manager_credentials.txt"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    const QStringList lines = QString::fromUtf8(file.readAll()).split(QRegularExpression("[\\r\\n]+"), Qt::SkipEmptyParts);
    if (lines.size() < 5 || lines[0] != "ARCANUM_MANAGER_AUTH" || lines[1] != "1")
        return false;

    const QString storedUsername = lines[2].trimmed();
    const QString salt = lines[3].trimmed();
    const QByteArray storedHash = lines[4].trimmed().toLatin1();
    const QByteArray source = (salt + "|" + password).toUtf8();
    const QByteArray calculated = QCryptographicHash::hash(source, QCryptographicHash::Sha256).toHex();
    return username == storedUsername && calculated == storedHash;
}

QWidget* AppMainWindow::createManagerPage() {
    auto* page = new QWidget;
    page->setObjectName("AppPage");
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(26, 22, 26, 22);
    root->setSpacing(14);

    auto* header = new QHBoxLayout;
    auto* back = actionButton("←  HOME");
    connect(back, &QPushButton::clicked, this, [this]() { returnToWelcome(); });
    header->addWidget(back);
    header->addStretch();
    header->addWidget(heading("MANAGEMENT DASHBOARD"));
    header->addStretch();
    auto* report = actionButton("GENERATE REPORT", true);
    connect(report, &QPushButton::clicked, this, [this]() {
        try {
            fileManager_.generateReport(manager_);
            fileManager_.writeLog("Manager generated report");
            QMessageBox::information(this, "Report generated",
                "The report has been saved to data/restaurant_report.txt.");
        }
        catch (const std::exception& error) {
            QMessageBox::critical(this, "Report error", QString::fromUtf8(error.what()));
        }
    });
    header->addWidget(report);
    root->addLayout(header);

    auto* tabs = new QTabWidget;
    tabs->setObjectName("ManagerTabs");

    // Overview.
    auto* overview = new QWidget;
    auto* overviewLayout = new QVBoxLayout(overview);
    overviewLayout->setContentsMargins(18, 18, 18, 18);
    auto* metrics = new QHBoxLayout;
    overviewOrdersLabel_ = new QLabel;
    overviewTablesLabel_ = new QLabel;
    overviewMenuLabel_ = new QLabel;
    for (QLabel* label : { overviewOrdersLabel_, overviewTablesLabel_, overviewMenuLabel_ }) {
        label->setObjectName("MetricCard");
        label->setMinimumHeight(120);
        label->setAlignment(Qt::AlignCenter);
        metrics->addWidget(label, 1);
    }
    overviewLayout->addLayout(metrics);
    auto* overviewNote = new QLabel("Restaurant metrics are calculated from the local database.\n"
                                    "Order and table changes are automatically saved in the data folder.");
    overviewNote->setObjectName("BodyText");
    overviewNote->setWordWrap(true);
    overviewLayout->addWidget(overviewNote);
    overviewLayout->addStretch();
    tabs->addTab(overview, "Overview");

    // Orders.
    auto* ordersPage = new QWidget;
    auto* ordersLayout = new QVBoxLayout(ordersPage);
    managerOrdersTable_ = new QTableWidget(0, 10);
    managerOrdersTable_->setHorizontalHeaderLabels({ "#", "Customer", "Guests", "Table", "Status", "Subtotal", "Discount", "Points used", "Points earned", "Amount due" });
    configureTable(managerOrdersTable_);
    managerOrdersTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ordersLayout->addWidget(managerOrdersTable_);
    auto* workflowHint = new QLabel("Order workflow: New → Preparing → Ready → Served. New, Preparing and Ready orders may be cancelled. Served and Cancelled are final states.");
    workflowHint->setObjectName("HintText");
    workflowHint->setWordWrap(true);
    ordersLayout->addWidget(workflowHint);
    auto* orderActions = new QHBoxLayout;
    startPreparationButton_ = actionButton("START PREPARATION", true);
    markReadyButton_ = actionButton("MARK AS READY");
    markServedButton_ = actionButton("MARK AS SERVED");
    cancelOrderButton_ = actionButton("CANCEL ORDER");
    startPreparationButton_->setEnabled(false);
    markReadyButton_->setEnabled(false);
    markServedButton_->setEnabled(false);
    cancelOrderButton_->setEnabled(false);
    connect(startPreparationButton_, &QPushButton::clicked, this, [this]() { changeSelectedOrderStatus(static_cast<int>(OrderStatus::Preparing)); });
    connect(markReadyButton_, &QPushButton::clicked, this, [this]() { changeSelectedOrderStatus(static_cast<int>(OrderStatus::Ready)); });
    connect(markServedButton_, &QPushButton::clicked, this, [this]() { changeSelectedOrderStatus(static_cast<int>(OrderStatus::Served)); });
    connect(cancelOrderButton_, &QPushButton::clicked, this, [this]() { changeSelectedOrderStatus(static_cast<int>(OrderStatus::Cancelled)); });
    connect(managerOrdersTable_, &QTableWidget::itemSelectionChanged, this, [this]() { updateOrderActionButtons(); });
    orderActions->addWidget(startPreparationButton_);
    orderActions->addWidget(markReadyButton_);
    orderActions->addWidget(markServedButton_);
    orderActions->addWidget(cancelOrderButton_);
    ordersLayout->addLayout(orderActions);
    tabs->addTab(ordersPage, "Orders");

    // Tables.
    auto* tablesPage = new QWidget;
    auto* tablesLayout = new QVBoxLayout(tablesPage);
    managerTablesTable_ = new QTableWidget(0, 4);
    managerTablesTable_->setHorizontalHeaderLabels({ "Table", "Capacity", "Seats occupied", "Status" });
    configureTable(managerTablesTable_);
    tablesLayout->addWidget(managerTablesTable_);
    auto* tableActions = new QHBoxLayout;
    auto* addTableButton = actionButton("ADD TABLE", true);
    auto* removeTableButton = actionButton("REMOVE SELECTED");
    connect(addTableButton, &QPushButton::clicked, this, [this]() { addDiningTable(); });
    connect(removeTableButton, &QPushButton::clicked, this, [this]() { removeSelectedDiningTable(); });
    tableActions->addWidget(addTableButton);
    tableActions->addWidget(removeTableButton);
    tableActions->addStretch();
    tablesLayout->addLayout(tableActions);
    tabs->addTab(tablesPage, "Tables");

    // Menu and ingredients.
    auto* menuPage = new QWidget;
    auto* menuLayout = new QVBoxLayout(menuPage);
    auto* managerMenuTabs = new QTabWidget;
    managerMenuTable_ = new QTableWidget(0, 4);
    managerMenuTable_->setHorizontalHeaderLabels({ "Name", "Type", "Category", "Price" });
    configureTable(managerMenuTable_);
    managerMenuTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    managerMenuTabs->addTab(managerMenuTable_, "Main Menu");

    auto* seasonalAdminPage = new QWidget;
    auto* seasonalAdminLayout = new QVBoxLayout(seasonalAdminPage);
    auto* seasonalAdminHint = new QLabel;
    seasonalAdminHint->setObjectName("HintText");
    seasonalAdminHint->setWordWrap(true);
    const SeasonalMenu& configuredSeasonalMenu = manager_.getSeasonalMenu();
    if (configuredSeasonalMenu.isAvailable()) {
        seasonalAdminHint->setText("The seasonal menu is currently active.");
    }
    else if (configuredSeasonalMenu.getDayBegin() > 0) {
        seasonalAdminHint->setText(QString("Inactive period: %1.%2.%3 – %4.%5.%6.")
            .arg(configuredSeasonalMenu.getDayBegin(), 2, 10, QChar('0'))
            .arg(configuredSeasonalMenu.getMonthBegin(), 2, 10, QChar('0'))
            .arg(configuredSeasonalMenu.getYearBegin())
            .arg(configuredSeasonalMenu.getDayEnd(), 2, 10, QChar('0'))
            .arg(configuredSeasonalMenu.getMonthEnd(), 2, 10, QChar('0'))
            .arg(configuredSeasonalMenu.getYearEnd()));
    }
    else {
        seasonalAdminHint->setText("No seasonal availability period has been configured.");
    }
    seasonalAdminLayout->addWidget(seasonalAdminHint);
    managerSeasonalMenuTable_ = new QTableWidget(0, 4);
    managerSeasonalMenuTable_->setHorizontalHeaderLabels({ "Name", "Type", "Category", "Price" });
    configureTable(managerSeasonalMenuTable_);
    managerSeasonalMenuTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    seasonalAdminLayout->addWidget(managerSeasonalMenuTable_, 1);
    managerMenuTabs->addTab(seasonalAdminPage, "Seasonal Menu");
    menuLayout->addWidget(managerMenuTabs, 1);
    auto* menuNote = new QLabel("Main and seasonal menu items are managed separately in the restaurant database. Seasonal items can be ordered only during their configured availability period.");
    menuNote->setObjectName("HintText");
    menuNote->setWordWrap(true);
    menuLayout->addWidget(menuNote);
    tabs->addTab(menuPage, "Menu");

    auto* ingredientsPage = new QWidget;
    auto* ingredientsLayout = new QVBoxLayout(ingredientsPage);
    managerIngredientsTable_ = new QTableWidget(0, 5);
    managerIngredientsTable_->setHorizontalHeaderLabels({ "Ingredient", "Unit", "Calories", "Protein", "Fat / carbs" });
    configureTable(managerIngredientsTable_);
    managerIngredientsTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ingredientsLayout->addWidget(managerIngredientsTable_);
    tabs->addTab(ingredientsPage, "Ingredients");

    auto* peoplePage = new QWidget;
    auto* peopleLayout = new QVBoxLayout(peoplePage);
    managerPeopleTable_ = new QTableWidget(0, 4);
    managerPeopleTable_->setHorizontalHeaderLabels({ "Full name", "Phone", "Type", "Details" });
    configureTable(managerPeopleTable_);
    managerPeopleTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    peopleLayout->addWidget(managerPeopleTable_);
    tabs->addTab(peoplePage, "Customers & Staff");

    root->addWidget(tabs, 1);
    return page;
}

void AppMainWindow::refreshManagerDashboard() {
    refreshManagerOrders();
    refreshManagerTables();
    refreshManagerMenu();
    refreshManagerIngredients();
    refreshManagerPeople();

    if (overviewOrdersLabel_)
        overviewOrdersLabel_->setText(QString("ORDERS\n\n%1").arg(manager_.getOrdersCount()));
    if (overviewTablesLabel_)
        overviewTablesLabel_->setText(QString("AVAILABLE TABLES\n\n%1 / %2")
            .arg(static_cast<qulonglong>(manager_.getDiningRoom().getAvailableTableCount()))
            .arg(static_cast<qulonglong>(manager_.getDiningRoom().getTableCount())));
    if (overviewMenuLabel_) {
        const auto items = manager_.getAvailableMenuItems();
        overviewMenuLabel_->setText(QString("MENU ITEMS\n\n%1").arg(static_cast<qulonglong>(items.size())));
    }
}

void AppMainWindow::refreshManagerOrders() {
    if (!managerOrdersTable_)
        return;
    managerOrdersTable_->setRowCount(0);
    const auto& orders = manager_.getOrders();
    for (int index = 0; index < static_cast<int>(orders.size()); ++index) {
        const Order& order = orders[static_cast<std::size_t>(index)];
        const int row = managerOrdersTable_->rowCount();
        managerOrdersTable_->insertRow(row);
        Person* person = manager_.findPerson(order.getClientPhoneNumber());
        managerOrdersTable_->setItem(row, 0, new QTableWidgetItem(QString::number(index + 1)));
        managerOrdersTable_->setItem(row, 1, new QTableWidgetItem(person ? q(person->getFullName()) : q(order.getClientPhoneNumber())));
        managerOrdersTable_->setItem(row, 2, new QTableWidgetItem(QString::number(order.getGuestCount())));
        managerOrdersTable_->setItem(row, 3, new QTableWidgetItem(order.hasAssignedTable() ? QString::number(order.getTableNumber()) : "—"));
        auto* statusCell = new QTableWidgetItem(orderStatusText(order.getStatus()));
        if (order.getStatus() == OrderStatus::Served)
            statusCell->setForeground(QColor("#83bd9d"));
        else if (order.getStatus() == OrderStatus::Cancelled)
            statusCell->setForeground(QColor("#d97777"));
        else if (order.getStatus() == OrderStatus::Ready)
            statusCell->setForeground(QColor("#d6b56d"));
        else if (order.getStatus() == OrderStatus::Preparing)
            statusCell->setForeground(QColor("#8db8d8"));
        managerOrdersTable_->setItem(row, 4, statusCell);
        managerOrdersTable_->setItem(row, 5, new QTableWidgetItem(money(order.calculateTotal())));
        managerOrdersTable_->setItem(row, 6, new QTableWidgetItem(money(order.getLoyaltyDiscount())));
        managerOrdersTable_->setItem(row, 7, new QTableWidgetItem(QString::number(order.getRedeemedBonusPoints())));
        const int earnedPoints = order.getStatus() == OrderStatus::Served
            ? static_cast<int>(std::floor(order.calculatePayableTotal() + 0.0001f)) * LoyaltyPolicy::pointsEarnedPerByn
            : 0;
        managerOrdersTable_->setItem(row, 8, new QTableWidgetItem(QString::number(earnedPoints)));
        managerOrdersTable_->setItem(row, 9, new QTableWidgetItem(money(order.calculatePayableTotal())));
    }
    updateOrderActionButtons();
}

void AppMainWindow::refreshManagerTables() {
    if (!managerTablesTable_)
        return;
    managerTablesTable_->setRowCount(0);
    const auto& tables = manager_.getDiningRoom().getTables();
    for (const auto& pair : tables) {
        const DiningTable& table = pair.second;
        const int row = managerTablesTable_->rowCount();
        managerTablesTable_->insertRow(row);
        managerTablesTable_->setItem(row, 0, new QTableWidgetItem(QString::number(table.getNumber())));
        managerTablesTable_->setItem(row, 1, new QTableWidgetItem(QString::number(table.getCapacity())));
        managerTablesTable_->setItem(row, 2, new QTableWidgetItem(QString::number(table.getOccupiedSeats())));
        auto* status = new QTableWidgetItem(table.isAvailable() ? "Available" : "Occupied");
        status->setForeground(table.isAvailable() ? QColor("#83bd9d") : QColor("#d97777"));
        managerTablesTable_->setItem(row, 3, status);
    }
}

void AppMainWindow::refreshManagerMenu() {
    auto populate = [this](QTableWidget* table, const std::vector<const MenuObject*>& items) {
        if (!table)
            return;
        table->setRowCount(0);
        for (const MenuObject* object : items) {
            if (!object)
                continue;
            const int row = table->rowCount();
            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(q(object->getTitle())));
            table->setItem(row, 1, new QTableWidgetItem(q(object->getType())));
            table->setItem(row, 2, new QTableWidgetItem(q(object->getCategory())));
            table->setItem(row, 3, new QTableWidgetItem(money(object->getPrice())));
        }
    };
    populate(managerMenuTable_, manager_.getMainMenu().getItems());
    populate(managerSeasonalMenuTable_, manager_.getSeasonalMenu().getItems());
}

void AppMainWindow::refreshManagerIngredients() {
    if (!managerIngredientsTable_)
        return;
    managerIngredientsTable_->setRowCount(0);
    for (const Ingredient& ingredient : manager_.getCatalog().getAll()) {
        const NutritionValue nutrition = ingredient.getNutrition();
        const int row = managerIngredientsTable_->rowCount();
        managerIngredientsTable_->insertRow(row);
        managerIngredientsTable_->setItem(row, 0, new QTableWidgetItem(q(ingredient.getName())));
        managerIngredientsTable_->setItem(row, 1, new QTableWidgetItem(ingredientUnit(ingredient.getUnit())));
        managerIngredientsTable_->setItem(row, 2, new QTableWidgetItem(QString::number(nutrition.getCalories(), 'f', 1)));
        managerIngredientsTable_->setItem(row, 3, new QTableWidgetItem(QString::number(nutrition.getProteins(), 'f', 1)));
        managerIngredientsTable_->setItem(row, 4, new QTableWidgetItem(
            QString("%1 / %2").arg(nutrition.getFats(), 0, 'f', 1).arg(nutrition.getCarbs(), 0, 'f', 1)));
    }
}

void AppMainWindow::refreshManagerPeople() {
    if (!managerPeopleTable_)
        return;
    managerPeopleTable_->setRowCount(0);
    const auto& people = manager_.getPeople();
    for (size_t index = 0; index < people.getSize(); ++index) {
        const Person* person = people.get(index);
        if (!person)
            continue;
        const int row = managerPeopleTable_->rowCount();
        managerPeopleTable_->insertRow(row);
        managerPeopleTable_->setItem(row, 0, new QTableWidgetItem(q(person->getFullName())));
        managerPeopleTable_->setItem(row, 1, new QTableWidgetItem(q(person->getPhoneNumber())));
        managerPeopleTable_->setItem(row, 2, new QTableWidgetItem(q(person->getType())));
        QString additional;
        if (const auto* client = dynamic_cast<const Client*>(person))
            additional = QString("Loyalty points: %1").arg(client->getBonus());
        else if (const auto* employee = dynamic_cast<const Employee*>(person))
            additional = q(employee->getPosition()) + " • " + q(employee->getShift());
        managerPeopleTable_->setItem(row, 3, new QTableWidgetItem(additional));
    }
}

void AppMainWindow::updateOrderActionButtons() {
    const bool hasSelection = managerOrdersTable_
        && managerOrdersTable_->selectionModel()
        && managerOrdersTable_->selectionModel()->hasSelection();
    if (!hasSelection) {
        if (startPreparationButton_) startPreparationButton_->setEnabled(false);
        if (markReadyButton_) markReadyButton_->setEnabled(false);
        if (markServedButton_) markServedButton_->setEnabled(false);
        if (cancelOrderButton_) cancelOrderButton_->setEnabled(false);
        return;
    }

    const int row = managerOrdersTable_->selectionModel()->selectedRows().first().row();
    const OrderStatus status = manager_.getOrderStatus(row);
    const bool isNew = status == OrderStatus::New;
    const bool isPreparing = status == OrderStatus::Preparing;
    const bool isReady = status == OrderStatus::Ready;
    if (startPreparationButton_) startPreparationButton_->setEnabled(isNew);
    if (markReadyButton_) markReadyButton_->setEnabled(isPreparing);
    if (markServedButton_) markServedButton_->setEnabled(isReady);
    if (cancelOrderButton_) cancelOrderButton_->setEnabled(isNew || isPreparing || isReady);
}

void AppMainWindow::changeSelectedOrderStatus(int statusValue) {
    if (!managerOrdersTable_ || !managerOrdersTable_->selectionModel()->hasSelection()) {
        QMessageBox::information(this, "Select an order", "Select an order in the table first.");
        return;
    }

    const int row = managerOrdersTable_->selectionModel()->selectedRows().first().row();
    try {
        const OrderStatus currentStatus = manager_.getOrderStatus(row);
        const OrderStatus requestedStatus = static_cast<OrderStatus>(statusValue);
        if (requestedStatus == OrderStatus::Preparing && currentStatus != OrderStatus::New)
            throw InvalidOperationException("Only a new order can enter preparation.");
        if (requestedStatus == OrderStatus::Ready && currentStatus != OrderStatus::Preparing)
            throw InvalidOperationException("Only an order in preparation can be marked ready.");
        if (requestedStatus == OrderStatus::Served && currentStatus != OrderStatus::Ready)
            throw InvalidOperationException("Only a ready order can be marked as served.");
        if (requestedStatus == OrderStatus::Cancelled
            && currentStatus != OrderStatus::New
            && currentStatus != OrderStatus::Preparing
            && currentStatus != OrderStatus::Ready)
            throw InvalidOperationException("Only active orders can be cancelled.");

        int creditedPoints = 0;
        QString rewardedPhone;
        bool rewardEligible = false;
        float amountPaid = 0.0f;
        if (requestedStatus == OrderStatus::Served) {
            const Order& order = manager_.findOrder(row);
            rewardedPhone = QString::fromStdString(order.getClientPhoneNumber());
            amountPaid = order.calculatePayableTotal();
            rewardEligible = dynamic_cast<Client*>(manager_.findPerson(order.getClientPhoneNumber())) != nullptr;
        }

        manager_.setOrderStatus(row, requestedStatus);
        if (requestedStatus == OrderStatus::Served && rewardEligible) {
            if (auto* client = dynamic_cast<Client*>(manager_.findPerson(rewardedPhone.toStdString()))) {
                // One loyalty point is earned per full BYN actually paid after redemption.
                creditedPoints = static_cast<int>(std::floor(amountPaid + 0.0001f)) * LoyaltyPolicy::pointsEarnedPerByn;
                if (creditedPoints > 0)
                    client->setBonus(client->getBonus() + creditedPoints);
            }
        }
        QString action = "Updated status of order #" + QString::number(row + 1)
            + " to " + orderStatusText(requestedStatus);
        if (creditedPoints > 0)
            action += QString("; credited %1 loyalty points to %2").arg(creditedPoints).arg(rewardedPhone);
        if (!persist(action, true))
            throw InvalidOperationException("Could not save the changes. The current in-memory status has been updated; retry saving before closing.");

        refreshManagerDashboard();
        if (!currentCustomerPhone_.isEmpty())
            refreshCustomerProfile();
        if (creditedPoints > 0) {
            QMessageBox::information(this, "Loyalty points credited",
                QString("Order #%1 has been served. %2 loyalty points have been added to the customer account.")
                    .arg(row + 1).arg(creditedPoints));
        }
    }
    catch (const std::exception& error) {
        QMessageBox::critical(this, "Order update error", QString::fromUtf8(error.what()));
    }
}

void AppMainWindow::addDiningTable() {
    bool numberOk = false;
    const int number = QInputDialog::getInt(this, "New table", "Table number:", 8, 1, 999, 1, &numberOk);
    if (!numberOk)
        return;
    bool capacityOk = false;
    const int capacity = QInputDialog::getInt(this, "Capacity", "Number of seats:", 4, 1, 20, 1, &capacityOk);
    if (!capacityOk)
        return;
    try {
        manager_.addTable(number, capacity);
        persist(QString("Added table #%1").arg(number), true);
        refreshManagerDashboard();
    }
    catch (const std::exception& error) {
        QMessageBox::critical(this, "Could not add table", QString::fromUtf8(error.what()));
    }
}

void AppMainWindow::removeSelectedDiningTable() {
    if (!managerTablesTable_ || !managerTablesTable_->selectionModel()->hasSelection()) {
        QMessageBox::information(this, "Select a table", "Select a table in the list first.");
        return;
    }
    const int row = managerTablesTable_->selectionModel()->selectedRows().first().row();
    const int number = managerTablesTable_->item(row, 0)->text().toInt();
    if (QMessageBox::question(this, "Remove table",
        QString("Remove table #%1?").arg(number)) != QMessageBox::Yes)
        return;
    try {
        manager_.removeTable(number);
        persist(QString("Removed table #%1").arg(number), true);
        refreshManagerDashboard();
    }
    catch (const std::exception& error) {
        QMessageBox::critical(this, "Could not remove table", QString::fromUtf8(error.what()));
    }
}

bool AppMainWindow::persist(const QString& action, bool generateReport) {
    // The database is the source of truth. A log/report failure must not roll
    // back an order that has already been durably written to the database.
    try {
        fileManager_.saveDatabase(manager_);
    }
    catch (const std::exception& error) {
        QMessageBox::critical(this, "Database save error", QString::fromUtf8(error.what()));
        return false;
    }

    QStringList secondaryErrors;
    try {
        fileManager_.writeLog(action.toStdString());
    }
    catch (const std::exception& error) {
        secondaryErrors << QString("Activity log: %1").arg(QString::fromUtf8(error.what()));
    }
    if (generateReport) {
        try {
            fileManager_.generateReport(manager_);
        }
        catch (const std::exception& error) {
            secondaryErrors << QString("Report: %1").arg(QString::fromUtf8(error.what()));
        }
    }
    if (!secondaryErrors.isEmpty())
        QMessageBox::warning(this, "Data saved with warnings", secondaryErrors.join("\n"));
    return true;
}

void AppMainWindow::applyCustomerToOrderForm() {
    if (!firstNameEdit_ || !lastNameEdit_ || !middleNameEdit_ || !phoneEdit_)
        return;

    if (currentCustomerPhone_.isEmpty()) {
        firstNameEdit_->clear();
        lastNameEdit_->clear();
        middleNameEdit_->clear();
        phoneEdit_->clear();
        firstNameEdit_->setReadOnly(false);
        lastNameEdit_->setReadOnly(false);
        middleNameEdit_->setReadOnly(false);
        phoneEdit_->setReadOnly(false);
        refreshCart();
        return;
    }

    Person* person = manager_.findPerson(currentCustomerPhone_.toStdString());
    auto* client = dynamic_cast<Client*>(person);
    if (!client) {
        currentCustomerPhone_.clear();
        applyCustomerToOrderForm();
        return;
    }

    firstNameEdit_->setText(QString::fromStdString(client->getFirstName()));
    lastNameEdit_->setText(QString::fromStdString(client->getLastName()));
    middleNameEdit_->setText(QString::fromStdString(client->getMiddlelName()));
    phoneEdit_->setText(currentCustomerPhone_);
    firstNameEdit_->setReadOnly(true);
    lastNameEdit_->setReadOnly(true);
    middleNameEdit_->setReadOnly(true);
    phoneEdit_->setReadOnly(true);
    refreshCart();
}

void AppMainWindow::openCustomerAccount() {
    if (!currentCustomerPhone_.isEmpty()) {
        if (!customerPage_) {
            customerPage_ = createCustomerPage();
            pages_->addWidget(customerPage_);
        }
        refreshCustomerProfile();
        pages_->setCurrentWidget(customerPage_);
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Customer Account");
    dialog.setMinimumWidth(480);
    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(26, 24, 26, 24);
    layout->addWidget(heading("CUSTOMER ACCOUNT"));

    auto* tabs = new QTabWidget(&dialog);
    auto* signInPage = new QWidget(tabs);
    auto* signInForm = new QFormLayout(signInPage);
    auto* signInPhone = new QLineEdit;
    signInPhone->setPlaceholderText("+375291234567");
    signInPhone->setValidator(new QRegularExpressionValidator(QRegularExpression("\\+375[0-9]{0,9}"), signInPhone));
    auto* signInPassword = new QLineEdit;
    signInPassword->setEchoMode(QLineEdit::Password);
    signInPassword->setPlaceholderText("Password");
    signInForm->addRow("Phone number", signInPhone);
    signInForm->addRow("Password", signInPassword);
    auto* signInNote = new QLabel("Sign in to view your loyalty points and order history.");
    signInNote->setObjectName("HintText");
    signInNote->setWordWrap(true);
    signInForm->addRow(signInNote);

    auto* registerPage = new QWidget(tabs);
    auto* registerForm = new QFormLayout(registerPage);
    auto* firstName = new QLineEdit;
    auto* lastName = new QLineEdit;
    auto* middleName = new QLineEdit;
    auto* registerPhone = new QLineEdit;
    registerPhone->setPlaceholderText("+375291234567");
    registerPhone->setValidator(new QRegularExpressionValidator(QRegularExpression("\\+375[0-9]{0,9}"), registerPhone));
    auto* registerPassword = new QLineEdit;
    registerPassword->setEchoMode(QLineEdit::Password);
    registerPassword->setPlaceholderText("At least 8 characters");
    auto* confirmPassword = new QLineEdit;
    confirmPassword->setEchoMode(QLineEdit::Password);
    registerForm->addRow("First name", firstName);
    registerForm->addRow("Last name", lastName);
    registerForm->addRow("Middle name (optional)", middleName);
    registerForm->addRow("Phone number", registerPhone);
    registerForm->addRow("Password", registerPassword);
    registerForm->addRow("Confirm password", confirmPassword);
    auto* registerNote = new QLabel("If you have already placed an order with this phone number, you can link an account to that customer record.");
    registerNote->setObjectName("HintText");
    registerNote->setWordWrap(true);
    registerForm->addRow(registerNote);

    tabs->addTab(signInPage, "Sign In");
    tabs->addTab(registerPage, "Create Account");
    layout->addWidget(tabs);

    auto* actionRow = new QHBoxLayout;
    auto* submitButton = actionButton("SIGN IN", true);
    auto* closeButton = actionButton("CLOSE");
    actionRow->addWidget(submitButton);
    actionRow->addWidget(closeButton);
    layout->addLayout(actionRow);

    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(tabs, &QTabWidget::currentChanged, this, [submitButton](int index) {
        submitButton->setText(index == 0 ? "SIGN IN" : "CREATE ACCOUNT");
    });

    connect(submitButton, &QPushButton::clicked, this, [this, &dialog, tabs,
        signInPhone, signInPassword, firstName, lastName, middleName, registerPhone,
        registerPassword, confirmPassword]() {
        QString errorMessage;
        if (tabs->currentIndex() == 0) {
            const QString phone = signInPhone->text().trimmed();
            const QString password = signInPassword->text();
            if (!QRegularExpression("^\\+375[0-9]{9}$").match(phone).hasMatch()) {
                QMessageBox::warning(&dialog, "Invalid phone number", "Enter a phone number in the format +375XXXXXXXXX.");
                return;
            }
            QJsonObject accounts;
            if (!readCustomerAccounts(QDir(dataDirectory_).absoluteFilePath("customer_accounts.json"), accounts, &errorMessage)) {
                QMessageBox::critical(&dialog, "Account file error", errorMessage);
                return;
            }
            const QJsonObject record = accounts.value(phone).toObject();
            const QString salt = record.value("salt").toString();
            const QString storedHash = record.value("hash").toString();
            auto* client = dynamic_cast<Client*>(manager_.findPerson(phone.toStdString()));
            if (!client || salt.isEmpty() || storedHash.isEmpty()
                || customerPasswordHash(salt, password) != storedHash) {
                QMessageBox::warning(&dialog, "Sign-in failed", "The phone number or password is incorrect, or no account is registered for this phone number.");
                return;
            }
            currentCustomerPhone_ = phone;
            applyCustomerToOrderForm();
            dialog.accept();
            return;
        }

        const QString first = firstName->text().trimmed();
        const QString last = lastName->text().trimmed();
        const QString middle = middleName->text().trimmed();
        const QString phone = registerPhone->text().trimmed();
        const QString password = registerPassword->text();
        if (first.isEmpty() || last.isEmpty()) {
            QMessageBox::warning(&dialog, "Missing details", "Enter your first and last name.");
            return;
        }
        if (!QRegularExpression("^\\+375[0-9]{9}$").match(phone).hasMatch()) {
            QMessageBox::warning(&dialog, "Invalid phone number", "Enter a phone number in the format +375XXXXXXXXX.");
            return;
        }
        if (password.size() < 8) {
            QMessageBox::warning(&dialog, "Weak password", "Use a password containing at least 8 characters.");
            return;
        }
        if (password != confirmPassword->text()) {
            QMessageBox::warning(&dialog, "Password mismatch", "The passwords do not match.");
            return;
        }

        QJsonObject accounts;
        const QString accountPath = QDir(dataDirectory_).absoluteFilePath("customer_accounts.json");
        if (!readCustomerAccounts(accountPath, accounts, &errorMessage)) {
            QMessageBox::critical(&dialog, "Account file error", errorMessage);
            return;
        }
        if (accounts.contains(phone)) {
            QMessageBox::warning(&dialog, "Account already exists", "An account already exists for this phone number. Use the Sign In tab.");
            return;
        }

        Person* existingPerson = manager_.findPerson(phone.toStdString());
        Client* client = dynamic_cast<Client*>(existingPerson);
        bool newClientCreated = false;
        if (existingPerson && !client) {
            QMessageBox::warning(&dialog, "Phone number unavailable", "This phone number belongs to an employee record.");
            return;
        }
        if (client) {
            const bool sameFirst = QString::fromStdString(client->getFirstName()).compare(first, Qt::CaseInsensitive) == 0;
            const bool sameLast = QString::fromStdString(client->getLastName()).compare(last, Qt::CaseInsensitive) == 0;
            if (!sameFirst || !sameLast) {
                QMessageBox::warning(&dialog, "Customer record mismatch", "The name does not match the existing customer record for this phone number.");
                return;
            }
        }
        else {
            try {
                client = new Client(first.toStdString(), last.toStdString(), middle.toStdString(), phone.toStdString(), 0);
                manager_.addPerson(client);
                newClientCreated = true;
            }
            catch (const std::exception& error) {
                delete client;
                QMessageBox::critical(&dialog, "Account creation failed", QString::fromUtf8(error.what()));
                return;
            }
        }

        const QString salt = QUuid::createUuid().toString(QUuid::WithoutBraces);
        QJsonObject record;
        record.insert("salt", salt);
        record.insert("hash", customerPasswordHash(salt, password));
        accounts.insert(phone, record);
        if (!writeCustomerAccounts(accountPath, accounts, &errorMessage)) {
            if (newClientCreated) {
                try { manager_.removePerson(phone.toStdString()); } catch (...) {}
            }
            QMessageBox::critical(&dialog, "Account creation failed", errorMessage);
            return;
        }

        try {
            fileManager_.saveDatabase(manager_);
        }
        catch (const std::exception& error) {
            accounts.remove(phone);
            writeCustomerAccounts(accountPath, accounts, nullptr);
            if (newClientCreated) {
                try { manager_.removePerson(phone.toStdString()); fileManager_.saveDatabase(manager_); } catch (...) {}
            }
            QMessageBox::critical(&dialog, "Account creation failed", QString::fromUtf8(error.what()));
            return;
        }
        try {
            fileManager_.writeLog("Customer account created for " + phone.toStdString());
        }
        catch (const std::exception& error) {
            QMessageBox::warning(&dialog, "Activity log warning", QString::fromUtf8(error.what()));
        }

        currentCustomerPhone_ = phone;
        applyCustomerToOrderForm();
        dialog.accept();
    });

    if (dialog.exec() == QDialog::Accepted && !currentCustomerPhone_.isEmpty()) {
        if (!customerPage_) {
            customerPage_ = createCustomerPage();
            pages_->addWidget(customerPage_);
        }
        refreshCustomerProfile();
        pages_->setCurrentWidget(customerPage_);
    }
}

QWidget* AppMainWindow::createCustomerPage() {
    auto* page = new QWidget;
    page->setObjectName("AppPage");
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(16);

    auto* header = new QHBoxLayout;
    auto* backButton = actionButton("←  HOME");
    connect(backButton, &QPushButton::clicked, this, [this]() { returnToWelcome(); });
    header->addWidget(backButton);
    header->addStretch();
    header->addWidget(heading("MY ARCANUM ACCOUNT"));
    header->addStretch();
    auto* logoutButton = actionButton("SIGN OUT");
    connect(logoutButton, &QPushButton::clicked, this, [this]() {
        currentCustomerPhone_.clear();
        applyCustomerToOrderForm();
        pages_->setCurrentWidget(welcomePage_);
    });
    header->addWidget(logoutButton);
    root->addLayout(header);

    auto* summaryPanel = makePanel("Panel");
    auto* summaryLayout = new QVBoxLayout(summaryPanel);
    summaryLayout->setContentsMargins(24, 20, 24, 20);
    customerNameLabel_ = new QLabel;
    customerNameLabel_->setObjectName("SectionTitle");
    customerBonusLabel_ = new QLabel;
    customerBonusLabel_->setObjectName("TotalLabel");
    auto* bonusNote = new QLabel("Earn 1 point for every full BYN paid after the order is served. Redeem points at checkout: 100 points = 1 BYN discount.");
    bonusNote->setObjectName("HintText");
    bonusNote->setWordWrap(true);
    summaryLayout->addWidget(customerNameLabel_);
    summaryLayout->addWidget(customerBonusLabel_);
    summaryLayout->addWidget(bonusNote);
    root->addWidget(summaryPanel);

    root->addWidget(heading("MY ORDER HISTORY"));
    customerOrdersTable_ = new QTableWidget(0, 10);
    customerOrdersTable_->setHorizontalHeaderLabels({ "Order #", "Items", "Guests", "Table", "Status", "Subtotal", "Discount", "Points used", "Points earned", "Amount due" });
    configureTable(customerOrdersTable_);
    customerOrdersTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    root->addWidget(customerOrdersTable_, 1);
    return page;
}

void AppMainWindow::refreshCustomerProfile() {
    if (!customerNameLabel_ || !customerBonusLabel_ || !customerOrdersTable_ || currentCustomerPhone_.isEmpty())
        return;
    auto* client = dynamic_cast<Client*>(manager_.findPerson(currentCustomerPhone_.toStdString()));
    if (!client) {
        currentCustomerPhone_.clear();
        pages_->setCurrentWidget(welcomePage_);
        return;
    }

    customerNameLabel_->setText(QString("WELCOME, %1").arg(QString::fromStdString(client->getFullName())));
    customerBonusLabel_->setText(QString("LOYALTY BALANCE: %1 POINTS  •  %2 DISCOUNT VALUE")
        .arg(client->getBonus()).arg(money(static_cast<float>(client->getBonus()) / static_cast<float>(LoyaltyPolicy::pointsPerBynDiscount))));
    customerOrdersTable_->setRowCount(0);

    const auto& orders = manager_.getOrders();
    for (int index = 0; index < static_cast<int>(orders.size()); ++index) {
        const Order& order = orders[static_cast<std::size_t>(index)];
        if (order.getClientPhoneNumber() != currentCustomerPhone_.toStdString())
            continue;
        const int row = customerOrdersTable_->rowCount();
        customerOrdersTable_->insertRow(row);
        customerOrdersTable_->setItem(row, 0, new QTableWidgetItem(QString::number(index + 1)));
        QStringList itemNames;
        for (const OrderItem& item : order.getItems())
            itemNames << QString("%1 × %2").arg(item.amount).arg(q(item.name));
        customerOrdersTable_->setItem(row, 1, new QTableWidgetItem(itemNames.join("; ")));
        customerOrdersTable_->setItem(row, 2, new QTableWidgetItem(QString::number(order.getGuestCount())));
        customerOrdersTable_->setItem(row, 3, new QTableWidgetItem(order.hasAssignedTable() ? QString::number(order.getTableNumber()) : "—"));
        customerOrdersTable_->setItem(row, 4, new QTableWidgetItem(orderStatusText(order.getStatus())));
        customerOrdersTable_->setItem(row, 5, new QTableWidgetItem(money(order.calculateTotal())));
        customerOrdersTable_->setItem(row, 6, new QTableWidgetItem(money(order.getLoyaltyDiscount())));
        customerOrdersTable_->setItem(row, 7, new QTableWidgetItem(QString::number(order.getRedeemedBonusPoints())));
        const int earnedPoints = order.getStatus() == OrderStatus::Served
            ? static_cast<int>(std::floor(order.calculatePayableTotal() + 0.0001f)) * LoyaltyPolicy::pointsEarnedPerByn
            : 0;
        customerOrdersTable_->setItem(row, 8, new QTableWidgetItem(QString::number(earnedPoints)));
        customerOrdersTable_->setItem(row, 9, new QTableWidgetItem(money(order.calculatePayableTotal())));
    }
}

void AppMainWindow::openNutritionAdvisor() {
    QDialog dialog(this);
    dialog.setWindowTitle("Personal Nutrition Match");
    dialog.setMinimumSize(820, 620);
    auto* root = new QVBoxLayout(&dialog);
    root->setContentsMargins(22, 20, 22, 20);
    root->setSpacing(12);
    root->addWidget(heading("BUILD A MEAL THAT FITS YOUR GOALS"));
    auto* intro = new QLabel("Set your nutrition limits per serving. The advisor calculates calories, protein, fat and carbohydrates from the actual recipe ingredients in the restaurant database.");
    intro->setObjectName("HintText");
    intro->setWordWrap(true);
    root->addWidget(intro);

    auto* criteriaPanel = makePanel("Panel");
    auto* criteria = new QGridLayout(criteriaPanel);
    criteria->setContentsMargins(16, 14, 16, 14);
    auto makeNutritionInput = [](const QString& suffix, double maximum) {
        auto* input = new QDoubleSpinBox;
        input->setRange(0.0, maximum);
        input->setDecimals(1);
        input->setSingleStep(10.0);
        input->setValue(0.0);
        input->setSpecialValueText("No limit");
        input->setSuffix(suffix);
        return input;
    };
    auto* maxCalories = makeNutritionInput(" kcal", 5000.0);
    auto* minProtein = makeNutritionInput(" g", 500.0);
    auto* maxFats = makeNutritionInput(" g", 500.0);
    auto* maxCarbs = makeNutritionInput(" g", 1000.0);
    auto* excluded = new QLineEdit;
    excluded->setPlaceholderText("e.g. peanuts, milk, mushrooms");
    criteria->addWidget(new QLabel("Maximum calories"), 0, 0);
    criteria->addWidget(maxCalories, 0, 1);
    criteria->addWidget(new QLabel("Minimum protein"), 0, 2);
    criteria->addWidget(minProtein, 0, 3);
    criteria->addWidget(new QLabel("Maximum fat"), 1, 0);
    criteria->addWidget(maxFats, 1, 1);
    criteria->addWidget(new QLabel("Maximum carbohydrates"), 1, 2);
    criteria->addWidget(maxCarbs, 1, 3);
    criteria->addWidget(new QLabel("Exclude ingredients"), 2, 0);
    criteria->addWidget(excluded, 2, 1, 1, 3);
    root->addWidget(criteriaPanel);

    auto* searchRow = new QHBoxLayout;
    auto* findSetsButton = actionButton("FIND MEAL COMBINATIONS", true);
    auto* findDishesButton = actionButton("FIND INDIVIDUAL DISHES");
    searchRow->addWidget(findSetsButton);
    searchRow->addWidget(findDishesButton);
    searchRow->addStretch();
    root->addLayout(searchRow);

    auto* resultTabs = new QTabWidget;
    auto* setsList = new QListWidget;
    auto* dishesList = new QListWidget;
    resultTabs->addTab(setsList, "Meal combinations (2–3 items)");
    resultTabs->addTab(dishesList, "Individual menu items");
    root->addWidget(resultTabs, 1);

    auto* footer = new QHBoxLayout;
    auto* addButton = actionButton("ADD SELECTED TO BASKET", true);
    auto* closeButton = actionButton("CLOSE");
    footer->addStretch();
    footer->addWidget(addButton);
    footer->addWidget(closeButton);
    root->addLayout(footer);

    std::vector<OrderRecommendation> setRecommendations;
    std::vector<Recommendation> dishRecommendations;

    auto buildRequest = [this, maxCalories, minProtein, maxFats, maxCarbs, excluded]() {
        NutritionRequest request;
        if (maxCalories->value() > 0.0) request.setMaxCalories(static_cast<float>(maxCalories->value()));
        if (minProtein->value() > 0.0) request.setMinProteins(static_cast<float>(minProtein->value()));
        if (maxFats->value() > 0.0) request.setMaxFats(static_cast<float>(maxFats->value()));
        if (maxCarbs->value() > 0.0) request.setMaxCarbs(static_cast<float>(maxCarbs->value()));
        const QStringList excludedNames = excluded->text().split(',', Qt::SkipEmptyParts);
        for (const QString& name : excludedNames) {
            const QString trimmed = name.trimmed();
            if (!trimmed.isEmpty()) request.addExcludedIngredient(trimmed.toStdString());
        }
        return request;
    };

    connect(findSetsButton, &QPushButton::clicked, this, [this, &dialog, &setRecommendations, setsList, resultTabs, buildRequest]() {
        try {
            const NutritionRequest request = buildRequest();
            if (!request.hasCriteria()) {
                QMessageBox::information(&dialog, "Set nutrition goals", "Enter at least one nutrition limit or an ingredient to exclude.");
                return;
            }
            OrderBuilder builder;
            setRecommendations = builder.buildRecommendations(manager_, request, 2, 3, 8);
            setsList->clear();
            if (setRecommendations.empty()) {
                setsList->addItem("No meal combinations match these limits. Try relaxing one or more criteria.");
            }
            for (std::size_t i = 0; i < setRecommendations.size(); ++i) {
                const OrderRecommendation& recommendation = setRecommendations[i];
                const NutritionValue& nutrition = recommendation.getTotalNutrition();
                QStringList names;
                for (const OrderSelection& selection : recommendation.getItems())
                    names << q(selection.object->getTitle());
                const QString summary = QString("MATCH %1  |  %2  |  %3 kcal  |  P %4 g  •  F %5 g  •  C %6 g  |  %7  |  %8% match")
                    .arg(i + 1).arg(names.join(" + "))
                    .arg(nutrition.getCalories(), 0, 'f', 1)
                    .arg(nutrition.getProteins(), 0, 'f', 1)
                    .arg(nutrition.getFats(), 0, 'f', 1)
                    .arg(nutrition.getCarbs(), 0, 'f', 1)
                    .arg(money(recommendation.getTotalPrice()))
                    .arg(recommendation.getScore(), 0, 'f', 0);
                auto* item = new QListWidgetItem(summary, setsList);
                item->setData(Qt::UserRole, static_cast<int>(i));
            }
            resultTabs->setCurrentWidget(setsList);
        }
        catch (const std::exception& error) {
            QMessageBox::critical(&dialog, "Nutrition search error", QString::fromUtf8(error.what()));
        }
    });

    connect(findDishesButton, &QPushButton::clicked, this, [this, &dialog, &dishRecommendations, dishesList, resultTabs, buildRequest]() {
        try {
            const NutritionRequest request = buildRequest();
            if (!request.hasCriteria()) {
                QMessageBox::information(&dialog, "Set nutrition goals", "Enter at least one nutrition limit or an ingredient to exclude.");
                return;
            }
            NutritionAdvisor advisor;
            dishRecommendations = advisor.recommend(manager_, request);
            dishesList->clear();
            if (dishRecommendations.empty()) {
                dishesList->addItem("No individual menu items match these limits. Try relaxing one or more criteria.");
            }
            for (std::size_t i = 0; i < dishRecommendations.size(); ++i) {
                const Recommendation& recommendation = dishRecommendations[i];
                const NutritionValue& nutrition = recommendation.getNutrition();
                const MenuObject* object = recommendation.getMenuObject();
                const QString summary = QString("%1  |  %2 kcal  |  P %3 g  •  F %4 g  •  C %5 g  |  %6  |  %7% match")
                    .arg(q(object->getTitle()))
                    .arg(nutrition.getCalories(), 0, 'f', 1)
                    .arg(nutrition.getProteins(), 0, 'f', 1)
                    .arg(nutrition.getFats(), 0, 'f', 1)
                    .arg(nutrition.getCarbs(), 0, 'f', 1)
                    .arg(money(object->getPrice()))
                    .arg(recommendation.getScore(), 0, 'f', 0);
                auto* item = new QListWidgetItem(summary, dishesList);
                item->setData(Qt::UserRole, static_cast<int>(i));
            }
            resultTabs->setCurrentWidget(dishesList);
        }
        catch (const std::exception& error) {
            QMessageBox::critical(&dialog, "Nutrition search error", QString::fromUtf8(error.what()));
        }
    });

    connect(addButton, &QPushButton::clicked, this, [this, &dialog, resultTabs, setsList, dishesList, &setRecommendations, &dishRecommendations]() {
        if (resultTabs->currentWidget() == setsList) {
            QListWidgetItem* selected = setsList->currentItem();
            if (!selected || setRecommendations.empty()) {
                QMessageBox::information(&dialog, "Select a match", "Choose a meal combination first.");
                return;
            }
            const int index = selected->data(Qt::UserRole).toInt();
            if (index < 0 || static_cast<std::size_t>(index) >= setRecommendations.size()) return;
            for (const OrderSelection& selection : setRecommendations[static_cast<std::size_t>(index)].getItems())
                addToCart(selection.object, selection.amount);
        }
        else {
            QListWidgetItem* selected = dishesList->currentItem();
            if (!selected || dishRecommendations.empty()) {
                QMessageBox::information(&dialog, "Select a dish", "Choose an individual dish first.");
                return;
            }
            const int index = selected->data(Qt::UserRole).toInt();
            if (index < 0 || static_cast<std::size_t>(index) >= dishRecommendations.size()) return;
            addToCart(dishRecommendations[static_cast<std::size_t>(index)].getMenuObject(), 1);
        }
        refreshCart();
        dialog.accept();
    });
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    dialog.exec();
}

void AppMainWindow::returnToWelcome() {
    pages_->setCurrentWidget(welcomePage_);
}

void AppMainWindow::closeEvent(QCloseEvent* event) {
    try {
        fileManager_.saveDatabase(manager_);
    }
    catch (const std::exception& error) {
        const auto answer = QMessageBox::warning(this, "Database save error",
            QString("The restaurant database could not be saved:\n%1\n\nClose the application anyway?")
                .arg(QString::fromUtf8(error.what())),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (answer == QMessageBox::No) {
            event->ignore();
            return;
        }
        event->accept();
        return;
    }

    try { fileManager_.writeLog("Application closed"); }
    catch (const std::exception& error) {
        QMessageBox::warning(this, "Activity log warning", QString::fromUtf8(error.what()));
    }
    try { fileManager_.generateReport(manager_); }
    catch (const std::exception& error) {
        QMessageBox::warning(this, "Report warning", QString::fromUtf8(error.what()));
    }
    event->accept();
}

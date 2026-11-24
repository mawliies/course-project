#include "orderBuilder.h"
#include "AllExceptions.h"
#include <algorithm>

OrderRecommendation::OrderRecommendation(const std::vector<OrderSelection>& items, const NutritionValue& totalNutrition, float totalPrice, float score, const std::string& explanation) : items(items), totalNutrition(totalNutrition), totalPrice(totalPrice), score(score), explanation(explanation) {}

const std::vector<OrderSelection>& OrderRecommendation::getItems() const {
    return items;
}
const NutritionValue& OrderRecommendation::getTotalNutrition() const {
    return totalNutrition;
}
float OrderRecommendation::getTotalPrice() const {
    return totalPrice;
}
float OrderRecommendation::getScore() const {
    return score;
}
const std::string& OrderRecommendation::getExplanation() const {
    return explanation;
}

bool OrderBuilder::containsExcludedIngredient(const MenuObject* object, const NutritionRequest& request) const {
    if (object == nullptr)
        return true;
    const auto& ingredients = object->getRecipe().getIngredients();
    return std::any_of(ingredients.begin(),ingredients.end(),[&request](const RecipeIngredient& ingredient) {
        return request.isIngredientExcluded(ingredient.name);
    }
    );
}
bool OrderBuilder::satisfiesLimits(const NutritionValue& nutrition, const NutritionRequest& request) const {
    const float epsilon = 0.0001f;
    if (request.getMaxCalories() > 0 && nutrition.getCalories() > request.getMaxCalories() + epsilon)
        return false;
    if (request.getMinProteins() > 0 && nutrition.getProteins() + epsilon < request.getMinProteins())
        return false;
    if (request.getMaxFats() > 0 && nutrition.getFats() > request.getMaxFats() + epsilon)
        return false;
    if (request.getMaxCarbs() > 0 && nutrition.getCarbs() > request.getMaxCarbs() + epsilon)
        return false;
    return true;
}

float OrderBuilder::calculateScore(const NutritionValue& nutrition,const NutritionRequest& request) const {
    float totalScore = 0;
    int criteriaCount = 0;
    if (request.getMaxCalories() > 0) {
        float ratio = (request.getMaxCalories() - nutrition.getCalories()) / request.getMaxCalories();
        ratio = std::max(0.0f, std::min(1.0f, ratio));
        totalScore += 70.0f + 30.0f * ratio;
        ++criteriaCount;
    }
    if (request.getMinProteins() > 0) {
        float ratio = (nutrition.getProteins() - request.getMinProteins()) /request.getMinProteins();
        ratio = std::max(0.0f, std::min(1.0f, ratio));
        totalScore += 70.0f + 30.0f * ratio;
        ++criteriaCount;
    }
    if (request.getMaxFats() > 0) {
        float ratio = (request.getMaxFats() - nutrition.getFats()) / request.getMaxFats();
        ratio = std::max(0.0f, std::min(1.0f, ratio));
        totalScore += 70.0f + 30.0f * ratio;
        ++criteriaCount;
    }
    if (request.getMaxCarbs() > 0) {
        float ratio =(request.getMaxCarbs() - nutrition.getCarbs()) /request.getMaxCarbs();
        ratio = std::max(0.0f, std::min(1.0f, ratio));
        totalScore += 70.0f + 30.0f * ratio;
        ++criteriaCount;
    }
    if (criteriaCount == 0)
        return 100.0f;
    return totalScore / static_cast<float>(criteriaCount);
}
std::string OrderBuilder::buildExplanation() const {
    return "All selected items satisfy the requested nutrition limits and excluded ingredient restrictions.";
}

void OrderBuilder::searchCombinations(const std::vector<Candidate>& candidates, const NutritionRequest& request, std::size_t minItems, std::size_t maxItems, std::size_t startIndex, std::vector<OrderSelection>& selected, float calories, float proteins, float fats, float carbs, float price, std::vector<OrderRecommendation>& results) const {
    if (selected.size() >= minItems) {
        NutritionValue totalNutrition(calories, proteins, fats, carbs);
        if (satisfiesLimits(totalNutrition, request)) {
            float score = calculateScore(totalNutrition, request);
            results.push_back(OrderRecommendation(selected, totalNutrition, price, score, buildExplanation()
            )
            );
        }
    }
    if (selected.size() >= maxItems)
        return;
    for (std::size_t i = startIndex; i < candidates.size(); ++i) {
        const Candidate& candidate = candidates[i];
        float newCalories = calories + candidate.nutrition.getCalories();
        float newProteins = proteins + candidate.nutrition.getProteins();
        float newFats = fats + candidate.nutrition.getFats();
        float newCarbs = carbs + candidate.nutrition.getCarbs();

        if (request.getMaxCalories() > 0 && newCalories > request.getMaxCalories() + 0.0001f)
            continue;
        if (request.getMaxFats() > 0 && newFats > request.getMaxFats() + 0.0001f)
            continue;
        if (request.getMaxCarbs() > 0 && newCarbs > request.getMaxCarbs() + 0.0001f)
            continue;
        selected.push_back(OrderSelection{ candidate.object, 1 });
        searchCombinations(candidates, request, minItems, maxItems, i + 1, selected, newCalories, newProteins, newFats, newCarbs, price + candidate.price, results);
        selected.pop_back();
    }
}

std::vector<OrderRecommendation> OrderBuilder::buildRecommendations(const RestaurantManager& manager, const NutritionRequest& request, std::size_t minItems, std::size_t maxItems, std::size_t maxResults) const {
    if (!request.hasCriteria())
        throw InvalidOperationException("Set at least one nutrition limit or excluded ingredient");
    if (minItems == 0 || maxItems < minItems)
        throw InvalidDataException("Invalid order combination size");
    if (maxItems > 5)
        throw InvalidDataException("Maximum number of positions in combination is 5");
    if (maxResults == 0)
        return {};

    const IngredientCatalog& catalog = manager.getCatalog();
    const std::vector<const MenuObject*> availableItems = manager.getAvailableMenuItems();
    std::vector<Candidate> candidates;
    candidates.reserve(availableItems.size());
    for (const MenuObject* object : availableItems) {
        if (object == nullptr)
            continue;
        if (containsExcludedIngredient(object, request))
            continue;
        NutritionValue nutrition = object->getRecipe().calculateNutrition(catalog);
        Candidate candidate = { object,nutrition, object->getPrice() };
        candidates.push_back(candidate);
    }
    if (candidates.size() < minItems)
        return {};
    if (maxItems > candidates.size())
        maxItems = candidates.size();
    std::vector<OrderRecommendation> results;
    std::vector<OrderSelection> selected;
    searchCombinations(candidates, request, minItems, maxItems, 0, selected, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, results);
    std::stable_sort(results.begin(), results.end(), [](const OrderRecommendation& first,const OrderRecommendation& second) {
        if (first.getScore() != second.getScore())
            return first.getScore() > second.getScore();
        return first.getTotalPrice() < second.getTotalPrice();
    }
    );
    if (results.size() > maxResults)
        results.erase(results.begin() + maxResults, results.end());
    return results;
}

Order OrderBuilder::createOrder(const OrderRecommendation& recommendation, const std::string& clientPhoneNumber) const {
    if (clientPhoneNumber.empty())
        throw InvalidDataException("Client phone number cannot be empty");

    if (recommendation.getItems().empty())
        throw InvalidOperationException("Cannot create order from empty recommendation");
    Order order(clientPhoneNumber, OrderStatus::Draft);
    for (const OrderSelection& selection : recommendation.getItems()) {
        if (selection.object == nullptr)
            throw InvalidDataException("Null menu object in recommendation");
        if (selection.amount <= 0)
            throw InvalidAmountException("Order item amount must be positive");
        order.addItem(*selection.object, selection.amount);
    }
    return order;
}
int OrderBuilder::confirmRecommendation(const std::vector<OrderRecommendation>& recommendations, std::size_t selectedIndex, const std::string& clientPhoneNumber, RestaurantManager& manager) const {
    if (selectedIndex >= recommendations.size())
        throw InvalidDataException("Selected recommendation does not exist");
    Order order = createOrder(recommendations[selectedIndex], clientPhoneNumber);
    int orderIndex = manager.getOrdersCount();
    manager.addOrder(order);
    return orderIndex;
}
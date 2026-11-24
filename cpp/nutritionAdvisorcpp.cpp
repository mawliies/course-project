#include "nutritionAdvisor.h"
#include "AllExceptions.h"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iomanip>

Recommendation::Recommendation(const MenuObject* menuObject, const NutritionValue& nutrition, float score, const std::string& explanation) : menuObject(menuObject), nutrition(nutrition), score(score), explanation(explanation) {}
const MenuObject* Recommendation::getMenuObject() const {
    return menuObject;
}
const NutritionValue& Recommendation::getNutrition() const {
    return nutrition;
}
float Recommendation::getScore() const {
    return score;
}
const std::string& Recommendation::getExplanation() const {
    return explanation;
}
bool NutritionAdvisor::isSuitable(const Recommendation& recommendation, const NutritionRequest& request) const {
    const NutritionValue& nutrition = recommendation.getNutrition();
    const float epsilon = 0.0001f;

    if (request.getMaxCalories() > 0.0f && nutrition.getCalories() > request.getMaxCalories() + epsilon)
        return false;
    if (request.getMinProteins() > 0.0f && nutrition.getProteins() + epsilon < request.getMinProteins())
        return false;
    if (request.getMaxFats() > 0.0f && nutrition.getFats() > request.getMaxFats() + epsilon)
        return false;
    if (request.getMaxCarbs() > 0.0f && nutrition.getCarbs() > request.getMaxCarbs() + epsilon)
        return false;
    const MenuObject* object = recommendation.getMenuObject();
    if (object == nullptr)
        return false;

    const std::vector<RecipeIngredient>& ingredients = object->getRecipe().getIngredients();
    const bool containsExcludedIngredient = std::any_of(ingredients.begin(), ingredients.end(), [&request](const RecipeIngredient& ingredient) {
            return request.isIngredientExcluded(ingredient.name);
        }
    );
    return !containsExcludedIngredient;
}
float NutritionAdvisor::calculateScore( const NutritionValue& nutrition,const NutritionRequest& request) const {
    float totalScore = 0.0f;
    int criteriaCount = 0;

    if (request.getMaxCalories() > 0.0f) {
        const float ratio = (request.getMaxCalories() - nutrition.getCalories()) / request.getMaxCalories();
        totalScore += 70.0f + 30.0f * ratio;
        ++criteriaCount;
    }
    if (request.getMinProteins() > 0.0f) {
        const float ratio = (nutrition.getProteins() - request.getMinProteins()) / request.getMinProteins();
        totalScore += 70.0f +30.0f * (ratio > 1.0f ? 1.0f : ratio);
        ++criteriaCount;
    }
    if (request.getMaxFats() > 0.0f) {
        const float ratio = (request.getMaxFats() - nutrition.getFats()) / request.getMaxFats();
        totalScore += 70.0f + 30.0f * ratio;
        ++criteriaCount;
    }
    if (request.getMaxCarbs() > 0.0f) {
        const float ratio =
            (request.getMaxCarbs() - nutrition.getCarbs()) /
            request.getMaxCarbs();

        totalScore += 70.0f + 30.0f * ratio;
        ++criteriaCount;
    }
    if (criteriaCount == 0)
        return 100.0f;
    return totalScore / static_cast<float>(criteriaCount);
}

std::string NutritionAdvisor::buildExplanation(const NutritionValue& nutrition, const NutritionRequest& request) const {
    std::ostringstream explanation;
    explanation << std::fixed << std::setprecision(1);
    explanation << "Meets the specified criteria. ";

    if (request.getMaxCalories() > 0.0f)
        explanation << "Calories: " << nutrition.getCalories() << "/" << request.getMaxCalories() << " kcal. ";
    if (request.getMinProteins() > 0.0f)
        explanation << "Protein: " << nutrition.getProteins() << " g (minimum " << request.getMinProteins() << " g). ";
    if (request.getMaxFats() > 0.0f)
        explanation << "Fats: " << nutrition.getFats() << "/" << request.getMaxFats() << " g. ";
    if (request.getMaxCarbs() > 0.0f)
        explanation << "Carbs: " << nutrition.getCarbs() << "/" << request.getMaxCarbs() << " g. ";
    if (!request.getExcludedIngredients().empty())
        explanation << "No excluded ingredients are present.";

    return explanation.str();
}

std::vector<Recommendation> NutritionAdvisor::recommend(const RestaurantManager& manager, const NutritionRequest& request) const {
    if (!request.hasCriteria())
        throw InvalidOperationException("Set at least one nutrition limit or excluded ingredient");

    const IngredientCatalog& catalog = manager.getCatalog();
    const std::vector<const MenuObject*> availableItems = manager.getAvailableMenuItems();
    std::vector<Recommendation> allCandidates;
    allCandidates.reserve(availableItems.size());
    for (const MenuObject* object : availableItems) {
        if (object == nullptr)
            continue;
        const NutritionValue nutrition = object->getRecipe().calculateNutrition(catalog);
        const float score = calculateScore(nutrition, request);
        const std::string explanation = buildExplanation(nutrition, request);
        allCandidates.push_back(Recommendation(object, nutrition, score, explanation));
    }
    std::vector<Recommendation> result;
    result.reserve(allCandidates.size());
    std::copy_if(allCandidates.begin(),allCandidates.end(),std::back_inserter(result),[this, &request](const Recommendation& candidate) {
            return isSuitable(candidate, request);
        }
    );
    std::sort(result.begin(), result.end(), [](const Recommendation& first, const Recommendation& second) {
        if (first.getScore() != second.getScore())
            return first.getScore() > second.getScore();
        if (first.getMenuObject()->getPrice() != second.getMenuObject()->getPrice())
            return first.getMenuObject()->getPrice() < second.getMenuObject()->getPrice();
        return first.getMenuObject()->getTitle() < second.getMenuObject()->getTitle();
    }
    );
    return result;
}

std::map<std::string, std::vector<Recommendation>> NutritionAdvisor::groupByCategory(const std::vector<Recommendation>& recommendations) const {
    std::map<std::string, std::vector<Recommendation>> grouped;
    for (const Recommendation& recommendation : recommendations) 
        if (recommendation.getMenuObject() != nullptr) 
            grouped[recommendation.getMenuObject()->getCategory()].push_back(recommendation);
    return grouped;
}

const Recommendation* NutritionAdvisor::findByTitle(const std::vector<Recommendation>& recommendations, const std::string& title) const {
    std::vector<Recommendation>::const_iterator found =std::find_if(recommendations.begin(),recommendations.end(),[&title](const Recommendation& recommendation) {
        return recommendation.getMenuObject() != nullptr &&recommendation.getMenuObject()->getTitle() == title;
    }
    );
    return found == recommendations.end() ? nullptr : &(*found);
}

const Recommendation* NutritionAdvisor::findLowestCalories(const std::vector<Recommendation>& recommendations) const {
    if (recommendations.empty())
        return nullptr;
    std::vector<Recommendation>::const_iterator found =std::min_element(recommendations.begin(),recommendations.end(),[](const Recommendation& first,const Recommendation& second) {
        return first.getNutrition().getCalories() < second.getNutrition().getCalories();
    }
    );
    return &(*found);
}

const Recommendation* NutritionAdvisor::findHighestProtein(const std::vector<Recommendation>& recommendations) const {
    if (recommendations.empty())
        return nullptr;
    std::vector<Recommendation>::const_iterator found = std::max_element(recommendations.begin(),recommendations.end(),[](const Recommendation& first,const Recommendation& second) {
        return first.getNutrition().getProteins() < second.getNutrition().getProteins();
    }
    );
    return &(*found);
}

std::size_t NutritionAdvisor::countAtLeastScore(const std::vector<Recommendation>& recommendations, float minimumScore) const {
    return static_cast<std::size_t>( std::count_if(recommendations.begin(), recommendations.end(), [minimumScore](const Recommendation& recommendation) {
                return recommendation.getScore() >= minimumScore;
    }
    )
    );
}

float NutritionAdvisor::calculateAverageScore(const std::vector<Recommendation>& recommendations) const {
    if (recommendations.empty())
        return 0.0f;
    float sum = 0.0f;
    std::for_each(recommendations.begin(),recommendations.end(),[&sum](const Recommendation& recommendation) {
        sum += recommendation.getScore();
    }
    );
    return sum / static_cast<float>(recommendations.size());
}

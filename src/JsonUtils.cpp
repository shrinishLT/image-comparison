#include "JsonUtils.h"

namespace JsonUtils {
     
     // convert 
    nlohmann::json to_json(const ExactComparisonResult& result) {
        nlohmann::json j;
        j["mismatchPercentage"] = result.mismatchPercentage;
        j["resultBuffer"] = result.resultBuffer;
        return j;
    }

    nlohmann::json to_json(const SmartIgnoreComparisonResult& result) {
        nlohmann::json j;
        j["baseResultBuffer"] = result.baseResultBuffer;
        j["compareResultBuffer"] = result.compareResultBuffer;
        j["compareMismatchPercentage"] = result.compareMismatchPercentage;
        j["baseMismatchPercentage"] = result.baseMismatchPercentage;
        return j;
    }
    
}

#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <nlohmann/json.hpp>
#include "Comparator/ImageComparator.h"  // Include other relevant headers as needed

namespace JsonUtils {

    // Convert ExactComparisonResult to JSON
    nlohmann::json to_json(const ExactComparisonResult& result);

    // Convert SmartIgnoreComparisonResult to JSON
    nlohmann::json to_json(const SmartIgnoreComparisonResult& result);

}

#endif // JSONUTILS_H

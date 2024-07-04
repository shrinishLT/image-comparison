// c_lib/interface.h
#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>
#include <vector>
#include "Comparator/ImageComparator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
        unsigned char* baseResultBuffer;
        int baseResultBufferSize;
        unsigned char* compareResultBuffer;
        int compareResultBufferSize;
        double compareMismatchPercentage;
        double baseMismatchPercentage;
} C_SmartIgnoreComparisonResult;

    C_SmartIgnoreComparisonResult smartIgnoreComparison(const char* baseImagePath, const char* compareImagePath);
    void freeSmartIgnoreComparisonResult(C_SmartIgnoreComparisonResult* result);


#ifdef __cplusplus
}
#endif

#endif // INTERFACE_H

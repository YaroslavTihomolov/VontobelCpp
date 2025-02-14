// CombinationIterator.h
#ifndef COMBINATION_H
#define COMBINATION_H

#include <vector>
#include <algorithm>

class Combination {
public:
    static long long total_combinations(int N, int K);

    Combination(std::vector<int> elements, int K);

    std::vector<int> next();

    std::vector<int> get_combination_at(long long index);

    void set_start_combination(std::vector<int> &combination);

private:
    int N, K;
    bool finish;
    std::vector<int> bitmask;
    std::vector<int> elements;

    static long long factorial(int n);
};

#endif // COMBINATION_H

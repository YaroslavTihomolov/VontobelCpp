#include "Combination.h"

Combination::Combination(std::vector<int> elements, int K) : elements(std::move(elements)), K(K), finish(false) {
    N = this->elements.size();
    bitmask = std::vector<int>(K, 1);
    bitmask.resize(N, 0);
}

std::vector<int> Combination::next() {
    std::vector<int> combination;

    if (finish) {
        return combination;
    }

    for (int i = 0; i < N; ++i) {
        if (bitmask[i]) {
            combination.push_back(elements[i]);
        }
    }

    if (!std::prev_permutation(bitmask.begin(), bitmask.end())) {
        finish = true;
    }

    return combination;
}

void Combination::set_start_combination(std::vector<int> &combination) {
    std::fill(bitmask.begin(), bitmask.end(), 0);
    for (const auto &value: combination) {
        bitmask[value] = 1;
    }
}

long long binomial_coefficient(int n, int k) {
    if (k > n) return 0;
    if (k == 0 || k == n) return 1;
    long long result = 1;
    for (int i = 1; i <= k; ++i) {
        result = result * (n - i + 1) / i;
    }
    return result;
}

std::vector<int> Combination::get_combination_at(long long index) {
    std::vector<int> combination;
    int start = 0;

    for (int i = 0; i < K; ++i) {
        for (int j = start; j < N; ++j) {
            long long count = binomial_coefficient(N - j - 1, K - i - 1);
            if (index < count) {
                combination.push_back(elements[j]);
                start = j + 1;
                break;
            } else {
                index -= count;
            }
        }
    }
    return combination;
}

long long Combination::total_combinations(int N, int K) {
    if (K > N - K) K = N - K;

    long long result = 1;
    for (int i = 1; i <= K; ++i) {
        result = result * (N - i + 1) / i;
    }
    return result;
}


#include "Vontobel.h"
#include <iostream>
#include <numeric>
#include <map>
#include <future>

Vontobel::Vontobel() {
    qc_offsets.resize(68);
    std::ranges::generate(qc_offsets, [n = 0]() mutable { return QC * n++; });
}

void Vontobel::solve(Matrix matrix) {
    std::vector<int> values_range(matrix.getCols() + 1);
    std::iota(values_range.begin(), values_range.end(), 0);

    auto required_columns = get_required_columns(matrix);

    long long combinations_count = Combination::total_combinations(matrix.getCols(), matrix.getRows() + 1) + 1;

    std::unordered_map<int, std::vector<std::vector<int>>> results;

    long long thread_count = 16;
    int chunk_index = 0;
    long long chunk_size = combinations_count / thread_count;

    std::vector<std::future<std::unordered_map<int, std::vector<std::vector<int>>>>> futures;

    while (chunk_index < thread_count) {
        if (chunk_index == thread_count - 1) {
            chunk_size = combinations_count - chunk_size * chunk_index;
        }
        auto future = std::async(
                std::launch::async,
                [this, &matrix, capture0 =
                combinations_count * chunk_index / thread_count, &chunk_size, &required_columns] {
                    return handle_chunk(matrix, capture0, chunk_size, required_columns);
                });
        futures.push_back(std::move(future));


        chunk_index++;
    }

    for (auto &future: futures) {
        auto chunk_result = future.get(); // Блокируем выполнение, пока задача не завершится

        for (const auto &[key, vec]: chunk_result) {
            if (results.find(key) == results.end()) {
                results[key] = vec;
            } else {
                results[key].insert(results[key].end(), vec.begin(), vec.end());
            }
        }
    }

    std::map<int, std::vector<std::vector<int>>> ordered_results(results.begin(), results.end());
    for (const auto &[key, vec]: ordered_results) {
        std::cout << key << ": " << vec.size() << std::endl;
    }
}

std::vector<std::set<int>> Vontobel::get_required_columns(Matrix &matrix) {
    int rows = matrix.getRows();
    int cols = matrix.getCols();
    std::vector<std::set<int>> required_columns;

    for (int i = 0; i < rows; ++i) {
        std::set<int> non_zero_indices;
        for (int j = 0; j < cols; ++j) {
            if (matrix.get(i, j) != -1) {
                non_zero_indices.insert(j);
            }
        }
        if (cols - non_zero_indices.size() >= rows) {
            required_columns.push_back(non_zero_indices);
        }
    }
    return required_columns;
}

std::unordered_map<int, std::vector<std::vector<int>>> Vontobel::handle_chunk(
        Matrix &matrix,
        long long start_index,
        long long &chunk_size,
        std::vector<std::set<int>> &required_columns) {
    std::unordered_map<int, std::vector<std::vector<int>>> local_results;
    std::vector<int> values_range(matrix.getCols());
    std::iota(values_range.begin(), values_range.end(), 0);

    Combination combinations(values_range, matrix.getRows() + 1);
    std::vector<int> combination = combinations.get_combination_at(start_index);
    combinations.set_start_combination(combination);

    int min_perm = INT32_MAX;

    for (int i = 0; i < chunk_size; ++i) {
        if (!has_intersection(required_columns, combination)) {
            continue;
        }

        int comb_sum = std::accumulate(combination.begin(), combination.end(), 0);

        Combination sub_combinations(combination, combination.size() - 1);
        int sum_perm = 0;

        std::vector<int> code_word;
        std::vector<int> sub_combination;
        while (!(sub_combination = sub_combinations.next()).empty()) {
            Matrix sub_matrix = matrix.get_sub_matrix(sub_combination);
            std::vector<std::vector<int>> sparse = sub_matrix.create_sparse_matrix();

            std::vector<uint8_t> used_cols(sub_matrix.dataCol[0].size(), false);
            std::bitset<QC> values;

            sub_matrix.count_permanent(sparse, used_cols, values);
            auto permanent_pol = getSetBitIndices(values);

            int sub_comb_sum = std::accumulate(sub_combination.begin(), sub_combination.end(), 0);
            int cyrc_index = comb_sum - sub_comb_sum;

            build_code_word(permanent_pol, cyrc_index);
            code_word.insert(code_word.end(), permanent_pol.begin(), permanent_pol.end());

            sum_perm += permanent_pol.size();
        }

        if (sum_perm != 0) {
            min_perm = std::min(min_perm, sum_perm);

            if (local_results.find(sum_perm) != local_results.end()) {
                local_results[sum_perm].push_back(code_word);
            } else {
                local_results[sum_perm] = {code_word};
            }
        }

        combination = combinations.next();
    }
    return local_results;
}

bool Vontobel::has_intersection(std::vector<std::set<int>> &required_columns, std::vector<int> &combination) {
    std::set<int> comb_set(combination.begin(), combination.end());
    std::set<int> intersection;

    for (const auto &required_column: required_columns) {
        std::set_intersection(comb_set.begin(), comb_set.end(),
                              required_column.begin(), required_column.end(),
                              std::inserter(intersection, intersection.begin()));

        if (!intersection.empty()) {
            return true;
        }
    }

    return false;
}

std::vector<int> Vontobel::getSetBitIndices(const std::bitset<QC> &bits) {
    std::vector<int> indices;
    for (int i = 0; i < QC; ++i) {
        if (bits[i]) {
            indices.push_back(i);
        }
    }
    return indices;
}

void Vontobel::build_code_word(std::vector<int> &values, int qc_index) {
    for (int &value: values) {
        value = qc_offsets[qc_index] + (QC - 1 - value);
    }
}

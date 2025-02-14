#include <iostream>
#include <chrono>
#include <sstream>
#include <bitset>
#include "Matrix.h"
#include "Vontobel.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.

const int SHIFT = 5;

void add_to_list(int val, std::vector<int> &values, int values_count) {
    size_t start = values.size() - values_count;
    for (size_t i = start; i < values.size(); ++i) {
        values[i] = (values[i] + val) % QC;
    }
}


std::vector<std::vector<int>> create_sparse_matrix(const std::vector<std::vector<int>> &matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();
    std::vector<std::vector<int>> sparse(rows);

    for (int row_idx = 0; row_idx < rows; ++row_idx) {
        for (int col_idx = 0; col_idx < cols; ++col_idx) {
            if (matrix[row_idx][col_idx] != -1) {
                sparse[row_idx].push_back(col_idx);
            }
        }
    }

    return sparse;
}

std::vector<int> remove_duplicates(const std::vector<int> &values) {
    std::unordered_map<int, int> element_counts;

    // Подсчёт количества каждого элемента
    for (int value: values) {
        element_counts[value]++;
    }

    // Вернуть только те элементы, которые встречаются нечетное количество раз
    std::vector<int> result;
    for (const auto &pair: element_counts) {
        if (pair.second % 2 != 0) {
            result.push_back(pair.first);
        }
    }

    return result;
}

// Функция для подсчета перманента
int count_permanent(const std::vector<std::vector<int>> &sparse, const std::vector<std::vector<int>> &matrix,
                    std::vector<bool> &used_cols, std::vector<int> &values, int step = 0) {
    if (step + 1 == matrix.size()) { // Проверка, находимся ли мы на последней строке матрицы
        for (int index: sparse[step]) { // Проходимся по столбцам, значения которых != 0 в последней строке
            if (used_cols[index] == 1) { // Если столбец уже был использован, пропускаем его
                continue;
            }
            values.push_back(matrix[step][index]); // Добавляем в массив
            return 1;
        }
        return 0;
    }

    int count = 0;
    for (int index: sparse[step]) {
        if (used_cols[index] == 0) {
            used_cols[index] = true;
            int values_add = count_permanent(sparse, matrix, used_cols, values, step + 1);
            add_to_list(matrix[step][index], values, values_add);
            count += values_add;
            used_cols[index] = false;
        }
    }

    return count;
}

int count_permanent2(const std::vector<std::vector<int>> &sparse, const std::vector<std::vector<int>> &matrix,
                     std::vector<bool> &used_cols, std::bitset<QC> &bitmask, int step = 0) {
    if (step + 1 == matrix.size()) { // Проверка, находимся ли мы на последней строке матрицы
        for (int index: sparse[step]) { // Проходимся по столбцам, значения которых != 0 в последней строке
            if (used_cols[index] == 1) { // Если столбец уже был использован, пропускаем его
                continue;
            }
            bitmask.flip(matrix[step][index]);
            return 1;
        }
        return 0;
    }

    int count = 0;
    for (int index: sparse[step]) {
        if (used_cols[index] == 0) {
            used_cols[index] = true;
            int values_add = count_permanent2(sparse, matrix, used_cols, bitmask, step + 1);
            int shift = matrix[step][index];
            bitmask = (bitmask >> shift) | (bitmask << (QC - shift));
            count += values_add;
            used_cols[index] = false;
        }
    }

    return count;
}

void count_permanent3(const std::vector<std::vector<int>> &sparse, const std::vector<std::vector<int>> &matrix,
                      std::vector<uint8_t> &used_cols, std::bitset<QC> &values, int accumulation = 0, int step = 0) {
    const int n = matrix.size();
    if (step + 1 == n) { // Проверка, находимся ли мы на последней строке матрицы
        for (int index: sparse[step]) { // Проходимся по столбцам, значения которых != 0 в последней строке
            if (!used_cols[index]) { // Если столбец уже был использован, пропускаем его
                values[(matrix[step][index] + accumulation) % QC].flip();
                break;
            }
        }
        return;
    }

    for (int index: sparse[step]) {
        if (!used_cols[index]) {
            used_cols[index] = 1;
            count_permanent3(
                    sparse, matrix,
                    used_cols,
                    values,
                    matrix[step][index] + accumulation,
                    step + 1);
            used_cols[index] = 0;
        }
    }
}

void printVector1(const std::vector<std::vector<int>> &vec) {
    for (const auto &row: vec) {
        for (const auto &elem: row) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<size_t> getSetBitIndices(const std::bitset<QC> &bits) {
    std::vector<size_t> indices;
    for (size_t i = 0; i < QC; ++i) {
        if (bits[i]) {
            indices.push_back(i);
        }
    }
    return indices;
}

int main() {

    /*std::chrono::milliseconds sum_duration;

    for (int i = 1; i < 100; ++i) {
        std::stringstream ss;
        ss << "matrices/matrix_" << i << ".txt";

        auto file = ss.str();

        Matrix base_matrix = Matrix(0, 0);
        base_matrix.loadFromFile(file);
        Matrix sub_matrix = base_matrix.getSubMatrix(14, 14);
        sub_matrix.sort_by_rows();
        sub_matrix.save_col_format();

        std::vector<bool> used_cols(sub_matrix.dataRow[0].size(), false);
        std::vector<uint8_t> used_cols2(sub_matrix.dataRow[0].size(), false);
        std::vector<int> values;
        values.reserve(sub_matrix.dataRow[0].size());
        auto sparse = create_sparse_matrix(sub_matrix.dataRow);
        std::bitset<QC> bitmask;

        std::bitset<QC> values2;

        auto start_time1 = std::chrono::high_resolution_clock::now();
        count_permanent(sparse, sub_matrix.dataRow, used_cols, values);
        values = remove_duplicates(values);
        auto end_time1 = std::chrono::high_resolution_clock::now();
        auto duration1 = duration_cast<std::chrono::milliseconds>(end_time1 - start_time1);
        std::cout << i << ". 1   " << duration1.count() << " " << values.size() << std::endl;

        auto start_time2 = std::chrono::high_resolution_clock::now();
        count_permanent3(sparse, sub_matrix.dataRow, used_cols2, values2);

        auto pol = getSetBitIndices(values2);
        auto end_time2 = std::chrono::high_resolution_clock::now();
        auto duration2 = duration_cast<std::chrono::milliseconds>(end_time2 - start_time2);
        std::cout << i << ". 2   " << duration2.count() << " " << pol.size() << "\n" << std::endl;
    }
    std::cout << " summary time " << sum_duration.count() << std::endl;*/

    Matrix base_matrix = Matrix(0, 0);
    base_matrix.loadFromFile("BG1_46x68_QC384.txt");
    Matrix sub_matrix = base_matrix.getSubMatrix(32, 10);
    sub_matrix.sort_by_rows();
    sub_matrix.save_col_format();

    Vontobel vontobel;

    auto start_time2 = std::chrono::high_resolution_clock::now();
    vontobel.solve(sub_matrix);
    auto end_time2 = std::chrono::high_resolution_clock::now();
    auto duration2 = duration_cast<std::chrono::milliseconds>(end_time2 - start_time2);
    std::cout << "Time: " << duration2.count() << std::endl;

    return 0;
}

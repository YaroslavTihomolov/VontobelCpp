#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <stdexcept>
#include <string>
#include <bitset>

const int QC = 384;

class Matrix {

private:
    size_t rows;
    size_t cols;
    std::vector<int> getRow(int row);

    std::vector<int> getCol(int col);

    int count_non_negative_elements(const std::vector<int> &row);

public:

    std::vector<std::vector<int>> dataCol;

    std::vector<std::vector<int>> dataRow;

    Matrix(size_t rows, size_t cols);

    explicit Matrix(size_t rows, size_t cols, const std::vector<std::vector<int>> &input);

    size_t getRows() const;

    size_t getCols() const;

    void setDataCol(std::vector<std::vector<int>> &data);

    Matrix get_sub_matrix(std::vector<int>);

    void sort_by_rows();

    double get(size_t row, size_t col) const;

    void set(size_t row, size_t col, double value);

    Matrix transpose() const;

    Matrix getSubMatrix(int col, int row);

    void save_col_format();

    void print() const;

    void printCol() const;

    void loadFromFile(const std::string &filename);

    std::vector<std::vector<int>> create_sparse_matrix();

    void count_permanent(const std::vector<std::vector<int>> &sparse, std::vector<uint8_t> &used_cols,
                         std::bitset<QC> &values, int accumulation = 0, int step = 0);
};

#endif // MATRIX_H

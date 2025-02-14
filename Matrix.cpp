#include "Matrix.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <bitset>

Matrix::Matrix(size_t rows, size_t cols) : rows(rows), cols(cols), dataRow(cols, std::vector<int>(rows, 0)) {}

Matrix::Matrix(size_t rows, size_t cols, const std::vector<std::vector<int>> &input) : rows(rows), cols(cols),
                                                                                       dataRow(input) {}

std::vector<int> Matrix::getRow(int row) {
    return dataRow[row];
}

std::vector<int> Matrix::getCol(int col) {
    return dataCol[col];
}

size_t Matrix::getRows() const {
    return rows;
}

size_t Matrix::getCols() const {
    return cols;
}

/*double Matrix::get(size_t row, size_t col) const {
    if (row >= rows || col >= cols) {
        throw std::out_of_range("Индекс за пределами матрицы");
    }
    return data[col][row];
}

void Matrix::set(size_t row, size_t col, double value) {
    if (row >= rows || col >= cols) {
        throw std::out_of_range("Индекс за пределами матрицы");
    }
    data[col][row] = value;
}*/

/*Matrix Matrix::transpose() const {
    Matrix transposed(rows, cols);
    for (size_t col = 0; col < cols; ++col) {
        for (size_t row = 0; row < rows; ++row) {
            transposed.set(row, col, get(row, col));
        }
    }
    return transposed;
}

void Matrix::print() const {
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            std::cout << get(row, col) << " ";
        }
        std::cout << std::endl;
    }
}*/

void Matrix::print() const {
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            std::cout << dataRow[row][col] << " ";
        }
        std::cout << std::endl;
    }
}

void Matrix::printCol() const {
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            std::cout << dataCol[col][row] << " ";
        }
        std::cout << std::endl;
    }
}

Matrix Matrix::getSubMatrix(int col, int row) {
    std::vector<std::vector<int>> sub_matrix;
    sub_matrix.reserve(col);
    for (int i = 0; i < row; ++i) {
        std::vector<int> original_vector = std::vector<int>(getRow(i));
        sub_matrix.emplace_back(original_vector.begin(), original_vector.begin() + col);
    }
    return Matrix(row, col, sub_matrix);
}


void Matrix::loadFromFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл");
    }

    std::string line;
    size_t tempCols = 0;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<int> row;
        int value;

        while (iss >> value) {
            row.push_back(value);
        }

        if (tempCols == 0) {
            tempCols = row.size();
        } else if (row.size() != tempCols) {
            throw std::runtime_error("Неправильный формат матрицы: строки имеют разную длину");
        }

        dataRow.push_back(row);
    }

    rows = dataRow.size();
    cols = tempCols;

    file.close();
}

void Matrix::sort_by_rows() {
    std::sort(dataRow.begin(), dataRow.end(), [this](const std::vector<int> &row1, const std::vector<int> &row2) {
        return count_non_negative_elements(row1) < count_non_negative_elements(row2);
    });

}


int Matrix::count_non_negative_elements(const std::vector<int> &row) {
    return std::count_if(row.begin(), row.end(), [](int elem) { return elem != -1; });
}

void Matrix::save_col_format() {
    dataCol.reserve(cols);
    for (int i = 0; i < cols; ++i) {
        std::vector<int> col;
        col.reserve(rows);
        for (int j = 0; j < rows; ++j) {
            col.emplace_back(dataRow[j][i]);
        }
        dataCol.emplace_back(col);
    }
}

void Matrix::setDataCol(std::vector<std::vector<int>> &data) {
    this->dataCol = data;
}

Matrix Matrix::get_sub_matrix(std::vector<int> cols) {
    std::vector<std::vector<int>> sub_matrix;
    sub_matrix.reserve(cols.size());
    for (int col: cols) {
        sub_matrix.emplace_back(getCol(col));
    }
    Matrix matrix(rows, cols.size());
    matrix.setDataCol(sub_matrix);
    return matrix;
}

double Matrix::get(size_t row, size_t col) const {
    return dataRow[row][col];
}

std::vector<std::vector<int>> Matrix::create_sparse_matrix() {
    int rows = dataCol.size();
    int cols = dataCol[0].size();
    std::vector<std::vector<int>> sparse(rows);

    for (int row_idx = 0; row_idx < rows; ++row_idx) {
        for (int col_idx = 0; col_idx < cols; ++col_idx) {
            if (dataCol[col_idx][row_idx] != -1) {
                sparse[row_idx].push_back(col_idx);
            }
        }
    }

    return sparse;
}

void Matrix::count_permanent(const std::vector<std::vector<int>> &sparse, std::vector<uint8_t> &used_cols,
                             std::bitset<QC> &values, int accumulation, int step) {
    const int n = dataCol.size();
    if (step + 1 == n) {
        for (int index: sparse[step]) {
            if (!used_cols[index]) {
                values[(dataCol[index][step] + accumulation) % QC].flip();
                break;
            }
        }
        return;
    }

    for (int index: sparse[step]) {
        if (!used_cols[index]) {
            used_cols[index] = 1;
            count_permanent(
                    sparse,
                    used_cols,
                    values,
                    dataCol[index][step] + accumulation,
                    step + 1);
            used_cols[index] = 0;
        }
    }
}

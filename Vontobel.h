#ifndef VONTOBEL_VONTOBEL_H
#define VONTOBEL_VONTOBEL_H


#include <set>
#include "Matrix.h"
#include "Combination.h"

class Vontobel {

private:
    std::vector<int> qc_offsets;

    std::vector<std::set<int>> get_required_columns(Matrix &matrix);

    std::vector<int> getSetBitIndices(const std::bitset<QC> &bits);

    std::unordered_map<int, std::vector<std::vector<int>>>
    handle_chunk(Matrix &matrix, long long start_index, long long &chunk_size, std::vector<std::set<int>> &required_columns);

    bool has_intersection(std::vector<std::set<int>> &required_columns, std::vector<int> &combination);

    void build_code_word(std::vector<int> &values, int qc_index);

public:
    explicit Vontobel();
    void solve(Matrix matrix);
};


#endif //VONTOBEL_VONTOBEL_H

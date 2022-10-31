#define A

#ifdef A
#include <vector>
#include <iostream>
#include <chrono>
#include <fstream>

// aliases used for better readability
using Row = std::vector<int>;
using Matrix = std::vector<Row>;

// generation of matrix with m rows and n columns, if type = 1 - fill with i+j used, i*j otherwise
Matrix generate(int m, int n, int type=1);
// returns the index where such element should be in the row, does not include higher boundary
// if boundaries not passed, search to whole row
int binaryRowSearch(const Row &row, int target, int s=-1, int e=-1);

// 1st algorithm, complexity O(n + m)
bool containsLin(const Matrix &matrix, int target);
// 2nd algorithm, complexity O(m * log n)
bool containsBin(const Matrix &matrix, int target);
// 3rd algorithm, complexity O(m * (log n - log m + 1))
bool containsExp(const Matrix &matrix, int target);

// returns the average time of 100 tests of algorithm passed by 'alg'
long long int timeOfAlg(bool (*alg)(const Matrix &, int), const Matrix &matrix, int target);

// performs all the tests, saves the data in file 'dat_alg.csv')
// column 'ratio' is not filled automatically
int main() {
    std::vector<bool (*)(const Matrix&, int)> algs {containsLin, containsBin, containsExp};
    std::ofstream output("../results/dat_alg.csv");
    output << "M; linear; binary; exponential 1; exponential 2; ratio \n";
    auto sep = "; ";

    int n = 1 << 13;
    for (int m = 2; m <= n; m <<= 1) {
        std::cout << m << "\n";
        auto matrix1 = generate(m, n), matrix2 = generate(m, n, 2);
        output << m << sep;
        for (auto &alg : algs) {
            output << timeOfAlg(alg, matrix1, 2 * n + 1) << sep;
        }
        output << timeOfAlg(containsExp, matrix2, 16 * n + 1) << "\n";
    }
    output.close();
    return 0;
}

Matrix generate(int m, int n, int type) {
    Matrix matrix(m, Row(n, 0));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i][j] = type == 1 ? (n / m * i + j) * 2 : n / m * i * j * 2;
        }
    }
    return matrix;
}

int binaryRowSearch(const Row &row, int target, int s, int e) {
    if (s < 0)
        s = 0;
    if (e > row.size() or e < 0)
        e = (int) row.size();

    while (s < e) {
        int m = (s + e) / 2;
        if (target == row[m])
            return m;

        if (target < row[m])
            e = m;
        else
            s = m + 1;
    }
    return e;
}

bool containsLin(const Matrix &matrix, int target) {
    int i = 0, j = (int) matrix[0].size() - 1;
    while (i < matrix.size() and j >= 0) {
        if (target == matrix[i][j])
            return true;

        if (target < matrix[i][j])
            --j;
        else
            ++i;
    }
    return false;
}

bool containsBin(const Matrix &matrix, int target) {
    for (auto &row : matrix) {
        auto i = binaryRowSearch(row, target);
        if (i < row.size() and row[i] == target)
            return true;
    }
    return false;
}

bool containsExp(const Matrix &matrix, int target) {
    int i = 0, j = (int) matrix[0].size() - 1;
    while (i < matrix.size() and j >= 0) {
        if (target == matrix[i][j])
            return true;

        if (target < matrix[i][j]) {
            int k = 1;
            while (j - k >= 0 and target < matrix[i][j - k])
                k *= 2;
            j = binaryRowSearch(matrix[i], target, j - k, j - 1);
        }
        else
            ++i;
    }
    return false;
}



long long int timeOfAlg(bool (*alg)(const Matrix &, int), const Matrix &matrix, int target) {
    using Clock = std::chrono::high_resolution_clock;
    using ns = std::chrono::nanoseconds;

    auto start = Clock::now();
    for (int i = 0; i < 100; ++i)
        alg(matrix, target);
    auto stop = Clock::now();
    return std::chrono::duration_cast<ns>(stop - start).count() / 100;
}
#endif
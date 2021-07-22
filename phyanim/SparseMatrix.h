#ifndef __PHYANIM_SPARSE_MATRIX__
#define __PHYANIM_SPARSE_MATRIX__

namespace phyanim
{
typedef std::pair<uint32_t, uint32_t> Index;

struct IndexHash
{
    std::size_t operator()(const Index& index) const
    {
        auto h1 = std::hash<uint32_t>{}(index.first);
        auto h2 = std::hash<uint32_t>{}(index.second);
        return h1 ^ h2;
    }
};

template <typename T>
using SMElement = std::pair<Index, T>;
template <typename T>
using SMElements = std::vector<SMElement<T>>;
template <class T>
using SMData = std::unordered_map<Index, T, IndexHash>;

/**
 * @brief The SparseMatrix class
 * Sparse Matrix
 */
template <uint32_t M,
          uint32_t N,
          typename T,
          typename =
              typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class SparseMatrix
{
public:
    /**
     * @brief SparseMatrix
     */
    SparseMatrix() { _data.reserve(M * N); };

public:
    /**
     * @brief
     * @param i
     * Row index
     * @return
     * A list with the elements in the row i
     */
    SMElements<T> row(uint32_t i) const
    {
        if (i >= M)
            throw std::out_of_range("Sparse matrix row index out of range");
        SMElements<T> elements;
        for (uint32_t j = 0; j < N; ++j)
        {
            Index index = {i, j};
            auto it = _data.find(index);
            if (it != _data.end()) elements.push_back(*it);
        }
        return elements;
    }

    /**
     * @brief
     * @param j
     * Column index
     * @return
     * A list with the elements in the column j
     */
    SMElements<T> col(uint32_t j) const
    {
        if (j >= N)
            throw std::out_of_range("Sparse matrix row index out of range");
        SMElements<T> elements;
        for (uint32_t i = 0; i < N; ++i)
        {
            Index index = {i, j};
            auto it = _data.find(index);
            if (it != _data.end()) elements.push_back(*it);
        }
        return elements;
    }

    /**
     * @brief
     * @return
     * Lists with the matrix elements sorted by rows
     */
    std::vector<SMElements<T>> rows() const
    {
        std::vector<SMElements<T>> rows(M);
        for (auto ele : _data)
        {
            rows[ele.first.first].push_back(ele);
        }
        return rows;
    }

    /**
     * @brief
     * @return
     * Lists with the matrix elements sorted by columns
     */
    std::vector<SMElements<T>> cols() const
    {
        std::vector<SMElements<T>> cols(N);
        for (auto ele : _data)
        {
            cols[ele.first.second].push_back(ele);
        }
        return cols;
    }
    /**
     * @brief print
     */
    void print() const
    {
        for (auto ele : _data)
            std::cout << "(" << ele.first.first << "," << ele.first.second
                      << ") = " << ele.second << std::endl;
    }

    /**
     * @brief printByRows
     */
    void printByRows() const
    {
        for (auto row : rows())
            for (auto ele : row)
                std::cout << "(" << ele.first.first << "," << ele.first.second
                          << ") = " << ele.second << std::endl;
    }

    /**
     * @brief printByCols
     */
    void printByCols() const
    {
        for (auto col : cols())
            for (auto ele : col)
                std::cout << "(" << ele.first.first << "," << ele.first.second
                          << ") = " << ele.second << std::endl;
    }

public:
    /**
     * @brief operator ()
     * @param i
     * @param j
     * @return
     */
    T operator()(uint32_t i, uint32_t j) const
    {
        if (i >= M)
            throw std::out_of_range("Sparse matrix row index out of range");

        if (j >= N)
            throw std::out_of_range("Sparse matrix column index out of range");

        Index index = {i, j};
        typename SMData<T>::const_iterator it = _data.find(index);
        if (it != _data.end())
        {
            return it->second;
        }
        else
        {
            return (T)0;
        }
    }

    /**
     * @brief operator ()
     * @param i
     * @param j
     * @return
     */
    T& operator()(uint32_t i, uint32_t j)
    {
        if (i >= M)
            throw std::out_of_range("Sparse matrix row index out of range");
        if (j >= N)
            throw std::out_of_range("Sparse matrix column index out of range");

        Index index = {i, j};
        typename SMData<T>::iterator it = _data.find(index);
        if (it != _data.end())
        {
            return it->second;
        }
        else
        {
            SMElement<T> ele = {index, (T)0};
            auto p = _data.insert(ele);
            return p.first->second;
        }
    }

    Vec<N, T> operator*(const Vec<N, T>& v) const
    {
        Vec<N, T> w(0);

        auto rows = this->rows();
        for (uint32_t i = 0; i < M; ++i)
            for (auto ele : rows[i]) w[i] += ele.second * v[ele.first.second];
        return w;
    }

private:
    /**
     * @brief sparse matrix data
     */
    SMData<T> _data;
};

/**
 * @brief The Conjugategradientsolver class
 * Conjugate Gradient Solver
 */
template <uint32_t N,
          typename T,
          typename =
              typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class ConjugateGradient
{
public:
    /**
     * @brief
     * Solve the A * x = b linear equation system through a Conjungate Gradient
     * approach
     * @param A
     * Matrix of MxN size
     * @param b
     * Vector of M size
     * @param threshold
     * Stop condition threshold
     * @param maxIterations
     * Stop condition maximun of iterations
     * @return
     * Vector of N size
     */
    Vec<N, T> solve(SparseMatrix<N, N, T> A,
                    Vec<N, T> b,
                    T threshold = 1e-10,
                    uint32_t maxIterations = 1000)
    {
        Vec<N, T> x(0);
        Vec<N, T> r = b - A * x;
        Vec<N, T> p = r;
        T rs_old = dot(r, r);
        Vec<N, T> Ap;
        uint32_t i;
        for (i = 1; i <= maxIterations; ++i)
        {
            Ap = A * p;
            T alpha = rs_old / dot(p, Ap);
            x = x + alpha * p;
            r = r - alpha * Ap;
            T rs_new = dot(r, r);
            if (rs_new < threshold) break;
            p = r + (rs_new / rs_old) * p;
            rs_old = rs_new;
        }
        std::cout << "Stopped at " << i << " iteration" << std::endl;
        return x;
    }
};

/**
 * @brief operator <<
 * @param out
 * @param m
 * @return
 */
template <uint32_t N, uint32_t M, typename T>
std::ostream& operator<<(std::ostream& out, const SparseMatrix<M, N, T>& m)
{
    for (uint32_t i = 0; i < M; ++i)
    {
        out << m(i, 0);
        for (uint32_t j = 1; j < N; ++j)
        {
            out << ", " << m(i, j);
        }
        if (i < M - 1) out << std::endl;
    }
    return out;
}

}  // namespace phyanim

#endif  // __PHYANIM_SPARSE_MATRIX__

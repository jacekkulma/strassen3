#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <algorithm>

template<class T>
class Matrix {
public:
	Matrix(const Matrix<T>& matrix) : m_isDataOwner(false), m_dataSize(0)
	{
        resize(matrix.m_size);
        *this = matrix;
	}

	Matrix(T padding = 0, int size = 0) : m_padding(padding), m_isDataOwner(false), m_dataSize(0)
	{
		resize(size);
	}

	Matrix(T* data, T padding, int dataSize, int rowsStart, int colsStart, int rowsEnd, int colsEnd, int size) : 
		m_data(data), m_padding(padding), m_dataSize(dataSize), m_isDataOwner(false), 
		m_rowsStart(rowsStart), m_colsStart(colsStart), m_rowsEnd(rowsEnd), m_colsEnd(colsEnd), m_size(size)
	{ }

	~Matrix()
	{
		if (m_isDataOwner) delete[] m_data;
	}

	Matrix<T>& operator=(const Matrix<T>& matrix) {
		if (m_size == 0) resize(matrix.m_size);
        if (m_size != matrix.m_size) throw std::runtime_error("Matrix sizes do not match.");

		m_padding = matrix.m_padding;

        for (int row = 0; row < m_size; row++) {
            for (int col = 0; col < m_size; col++) {
                set(row, col, matrix.get(row, col));
            }
        }

		return *this;
	}

	Matrix<T>& operator=(Matrix<T>&& matrix) {
		m_padding = matrix.m_padding;
        m_dataSize = matrix.m_dataSize;
        m_rowsStart = matrix.m_rowsStart;
        m_colsStart = matrix.m_colsStart;
        m_rowsEnd = matrix.m_rowsEnd;
        m_colsEnd = matrix.m_colsEnd;
        m_size = matrix.m_size;

        if (m_isDataOwner) delete[] m_data;
		m_data = matrix.m_data;

        m_isDataOwner = matrix.m_isDataOwner;
        matrix.m_isDataOwner = false;

		return *this;
	}

	void reserve(int size) 
	{
		if (m_isDataOwner) delete[] m_data;
		m_isDataOwner = true;
		m_dataSize = size;
		m_data = new T[m_dataSize * m_dataSize];
	}

	void resize(int size) 
	{
		m_rowsStart = m_colsStart = 0;
		m_rowsEnd = m_colsEnd = m_size = size;
		if (m_dataSize < m_size) reserve(m_size);
	}

	inline T& operator()(int row, int col) {
		return m_data[row * m_dataSize + col];
	}

	inline const T& operator()(int row, int col) const {
		return m_data[row * m_dataSize + col];
	}

	const T& get(int row, int col) const
	{
		row += m_rowsStart;
		col += m_colsStart;
		if (row >= m_rowsEnd || col >= m_colsEnd) return m_padding;
		return m_data[row * m_dataSize + col];
	}

	void set(int row, int col, const T& value)
	{
		row += m_rowsStart;
		col += m_colsStart;
		if (row < m_rowsEnd && col < m_colsEnd) {
			m_data[row * m_dataSize + col] = value;
		}
	}

	void emplace(int row, int col, T&& value)
	{
		row += m_rowsStart;
		col += m_colsStart;
		if (row < m_rowsEnd && col < m_colsEnd) {
			m_data[row * m_dataSize + col] = std::move(value);
		}
	}

	Matrix<Matrix<T>> partition(int count) const
	{
		int partitionedSize = (m_size + count - 1) / count;
		Matrix<Matrix<T>> matrix(Matrix<T>(m_padding), count);
		for (int row = 0; row < count; row++) {
			for (int col = 0; col < count; col++) {
				auto rowsStart = m_rowsStart + row * partitionedSize;
				auto colsStart = m_colsStart + col * partitionedSize;
				Matrix<T> submatrix(
					m_data, m_padding, m_dataSize,
					rowsStart, colsStart,
					std::min({ rowsStart + partitionedSize, m_rowsEnd }),
					std::min({ colsStart + partitionedSize, m_colsEnd }),
					partitionedSize
				);
				matrix.emplace(row, col, std::move(submatrix));
			}
		}
		return matrix;
	}

	Matrix<T>& operator+=(const Matrix<T>& rhs)& {
		if (m_size != rhs.m_size) throw std::runtime_error("Could not add matrices: operand sizes do not match.");
        auto rowCount = std::min({ m_rowsEnd - m_rowsStart, m_size });
        auto colCount = std::min({ m_colsEnd - m_colsStart, m_size });
        for (int row = 0; row < rowCount; row++) {
            for (int col = 0; col < colCount; col++) {
                m_data[(m_rowsStart + row) * m_dataSize + (m_colsStart + col)] += rhs.get(row, col);
            }
        }
		return *this;
	}

	friend Matrix<T> operator+(Matrix<T> lhs, const Matrix<T>& rhs) {
		lhs += rhs;
		return lhs;
	}

	Matrix<T>& operator-=(const Matrix<T>& rhs)& {
		if (m_size != rhs.m_size) throw std::runtime_error("Could not subtract matrices: operand sizes do not match.");
        auto rowCount = std::min({ m_rowsEnd - m_rowsStart, m_size });
        auto colCount = std::min({ m_colsEnd - m_colsStart, m_size });
        for (int row = 0; row < rowCount; row++) {
            for (int col = 0; col < colCount; col++) {
                m_data[(m_rowsStart + row) * m_dataSize + (m_colsStart + col)] -= rhs.get(row, col);
            }
        }
		return *this;
	}

	Matrix<T> operator-() const {
		Matrix<T> result(*this);
        auto rowCount = std::min({ m_rowsEnd - m_rowsStart, m_size });
        auto colCount = std::min({ m_colsEnd - m_colsStart, m_size });
        for (int row = 0; row < rowCount; row++) {
            for (int col = 0; col < colCount; col++) {
                result.set(row, col, -m_data[(m_rowsStart + row) * m_dataSize + (m_colsStart + col)]);
            }
        }
		return result;
	}

	friend Matrix<T> operator-(Matrix<T> lhs, const Matrix<T>& rhs) {
		lhs -= rhs;
		return lhs;
	}

	Matrix<T>& operator*=(const T& scalar)& {
        auto rowCount = std::min({ m_rowsEnd - m_rowsStart, m_size });
        auto colCount = std::min({ m_colsEnd - m_colsStart, m_size });
        for (int row = 0; row < rowCount; row++) {
            for (int col = 0; col < colCount; col++) {
                m_data[(m_rowsStart + row) * m_dataSize + (m_colsStart + col)] += scalar;
            }
        }
		return *this;
	}

	friend Matrix<T> operator*(Matrix<T> lhs, const T& scalar) {
		lhs *= scalar;
		return lhs;
	}

	friend Matrix<T> operator*(const T& scalar, Matrix<T> rhs) {
		rhs *= scalar;
		return rhs;
	}

	Matrix<T>& operator*=(const Matrix<T>& rhs)& {
		if (m_size != rhs.m_size) throw std::runtime_error("Could not multiply matrices: operand sizes do not match.");
		return *this = (*this) * rhs;
	}

	friend Matrix<T> operator*(const Matrix<T>& lhs, const Matrix<T>& rhs) {
		if (lhs.m_size != rhs.m_size) throw std::runtime_error("Could not multiply matrices: operand sizes do not match.");
		Matrix<T> result(lhs.m_padding, lhs.m_size);
		for (int i = 0; i < lhs.m_size; i++) {
			for (int j = 0; j < lhs.m_size; j++) {
				T sum = 0;
				for (int k = 0; k < lhs.m_size; k++) {
					sum += lhs.get(i, k) * rhs.get(k, j);
				}
				result.set(i, j, sum);
			}
		}
		return result;
	}

	friend Matrix<T> strassen3(const Matrix<T>& lhs, const Matrix<T>& rhs, int threshold = 1) {
		if (lhs.m_size != rhs.m_size) throw std::runtime_error("Could not multiply matrices: operand sizes do not match.");

        if (lhs.m_rowsStart >= lhs.m_rowsEnd || lhs.m_colsStart >= lhs.m_colsEnd ||
            rhs.m_rowsStart >= rhs.m_rowsEnd || rhs.m_colsStart >= rhs.m_colsEnd) {
            return Matrix<T>(nullptr, lhs.m_padding, 0, 0, 0, 0, 0, lhs.m_size);
        }

		// when matrices degenerated to scalar (m_size = 1) just "normal" multiplication
		if (lhs.m_size <= threshold) return lhs * rhs;

		// divide matrices to 9 (3x3) submatrices
		auto A = lhs.partition(3);
		const auto& A11 = A(0, 0);
		const auto& A12 = A(0, 1);
		const auto& A13 = A(0, 2);
		const auto& A21 = A(1, 0);
		const auto& A22 = A(1, 1);
		const auto& A23 = A(1, 2);
		const auto& A31 = A(2, 0);
		const auto& A32 = A(2, 1);
		const auto& A33 = A(2, 2);

		auto B = rhs.partition(3);
		const auto& B11 = B(0, 0);
		const auto& B12 = B(0, 1);
		const auto& B13 = B(0, 2);
		const auto& B21 = B(1, 0);
		const auto& B22 = B(1, 1);
		const auto& B23 = B(1, 2);
		const auto& B31 = B(2, 0);
		const auto& B32 = B(2, 1);
		const auto& B33 = B(2, 2);

		// calculate M_i submatrices (23 multiplications)
		Matrix<T> buf(A11.m_padding, A11.m_size);
		Matrix<T> buf2(B11.m_padding, B11.m_size);
		auto M1 = strassen3(buf.acc(A11, A12, A13).sub(A21, A22, A32, A33), B22, threshold);
		auto M2 = strassen3(buf.acc(A11).sub(A21), buf2.acc(B22).sub(B12), threshold);
		auto M3 = strassen3(A22, buf2.acc(B12, B21, B33).sub(B11, B22, B23, B31), threshold);
		auto M4 = strassen3(buf.acc(A21, A22).sub(A11), buf2.acc(B11, B22).sub(B12), threshold);
		auto M5 = strassen3(buf.acc(A21, A22), buf2.acc(B12).sub(B11), threshold);
		auto M6 = strassen3(A11, B11, threshold);
		auto M7 = strassen3(buf.acc(A31, A32).sub(A11), buf2.acc(B11, B23).sub(B13), threshold);
		auto M8 = strassen3(buf.acc(A31).sub(A11), buf2.acc(B13).sub(B23), threshold);
		auto M9 = strassen3(buf.acc(A31, A32), buf2.acc(B13).sub(B11), threshold);
		auto M10 = strassen3(buf.acc(A11, A12, A13).sub(A22, A23, A31, A32), B23, threshold);
		auto M11 = strassen3(A32, buf2.acc(B13, B21, B32).sub(B11, B22, B23, B31), threshold);
		auto M12 = strassen3(buf.acc(A32, A33).sub(A13), buf2.acc(B22, B31).sub(B32), threshold);
		auto M13 = strassen3(buf.acc(A13).sub(A33), buf2.acc(B22).sub(B32), threshold);
		auto M14 = strassen3(A13, B31, threshold);
		auto M15 = strassen3(buf.acc(A32, A33), buf2.acc(B32).sub(B31), threshold);
		auto M16 = strassen3(buf.acc(A22, A23).sub(A13), buf2.acc(B23, B31).sub(B33), threshold);
		auto M17 = strassen3(buf.acc(A13).sub(A23), buf2.acc(B23).sub(B33), threshold);
		auto M18 = strassen3(buf.acc(A22, A23), buf2.acc(B33).sub(B31), threshold);
		auto M19 = strassen3(A12, B21, threshold);
		auto M20 = strassen3(A23, B32, threshold);
		auto M21 = strassen3(A21, B13, threshold);
		auto M22 = strassen3(A31, B12, threshold);
		auto M23 = strassen3(A33, B33, threshold);

		// calculated C_ij submatrices
		Matrix<T> result(lhs.m_padding, lhs.m_size);

        auto C = result.partition(3);
        C(0, 0) = buf.acc(M6, M14, M19);
        C(0, 1) = buf.acc(M1, M4, M5, M6, M12, M14, M15);
        C(0, 2) = buf.acc(M6, M7, M9, M10, M14, M16, M18);
        C(1, 0) = buf.acc(M2, M3, M4, M6, M14, M16, M17);
        C(1, 1) = buf.acc(M2, M4, M5, M6, M20);
        C(1, 2) = buf.acc(M14, M16, M17, M18, M21);
        C(2, 0) = buf.acc(M6, M7, M8, M11, M12, M13, M14);
        C(2, 1) = buf.acc(M12, M13, M14, M15, M22);
        C(2, 2) = buf.acc(M6, M7, M8, M9, M23);

		return result;
	}

	friend std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix)
	{
		for (int row = 0; row < matrix.m_size; row++) {
			for (int col = 0; col < matrix.m_size; col++) {
				os << matrix.get(row, col) << " ";
			}
			os << "\n";
		}
		return os;
	}

	friend std::istream& operator>>(std::istream& is, Matrix<T>& matrix)
	{
        int size = 0, row = 0;
        for (std::string line; std::getline(is, line);) {
            std::istringstream lineStream(line);
            std::vector<T> values;
            T value;

            while (lineStream >> value) values.push_back(value);

            if (size == 0 && values.size() > 0) {
                size = values.size();
                matrix.resize(size);
            }
            if (size != values.size()) throw std::runtime_error("Could not read matrix: incorrect number of columns.");

            for (int col = 0; col < size; col++) matrix.set(row, col, values[col]);

            if (size > 0) row++;
            if (size > 0 && row >= size) return is;
        }
        if (row < size) throw std::runtime_error("Could not read matrix: incorrect number of rows.");
		return is;
	}

private:
	T* m_data;
	T m_padding;
	int m_dataSize;
	bool m_isDataOwner;
	int m_rowsStart; 
	int m_colsStart;
	int m_rowsEnd;
	int m_colsEnd;
	int m_size;

	Matrix<T>& acc(const Matrix<T>& tail) {
		return *this = tail;
	}

	template<typename... Args>
	Matrix<T>& acc(const Matrix<T>& head, const Args&... args) {
		return acc(args...) += head;
	}

	Matrix<T>& sub(const Matrix<T>& tail) {
		return *this -= tail;
	}

	template<typename... Args>
	Matrix<T>& sub(const Matrix<T>& head, const Args&... args) {
		return sub(args...) -= head;
	}
};

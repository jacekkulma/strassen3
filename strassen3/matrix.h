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
	
	const T& get(int row, int col) const
	{
		row += m_rowsStart;
		col += m_colsStart;
		if (row >= m_rowsEnd || col >= m_colsEnd) return m_padding;
		return m_data[row * m_dataSize + col];
	}

	void set(int row, int col, T value)
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
		for (int row = 0; row < m_size; row++) {
			for (int col = 0; col < m_size; col++) {
				set(row, col, get(row, col) + rhs.get(row, col));
			}
		}
		return *this;
	}

	friend Matrix<T> operator+(Matrix<T> lhs, const Matrix<T>& rhs) {
		lhs += rhs;
		return lhs;
	}

	Matrix<T>& operator-=(const Matrix<T>& rhs)& {
		for (int row = 0; row < m_size; row++) {
			for (int col = 0; col < m_size; col++) {
				set(row, col, get(row, col) - rhs.get(row, col));
			}
		}
		return *this;
	}

	Matrix<T> operator-() const {
		Matrix<T> result(*this);
		for (int row = 0; row < m_size; row++) {
			for (int col = 0; col < m_size; col++) {
				result.set(row, col, -get(row, col));
			}
		}
		return result;
	}

	friend Matrix<T> operator-(Matrix<T> lhs, const Matrix<T>& rhs) {
		lhs -= rhs;
		return lhs;
	}

	Matrix<T>& operator*=(const T& scalar)& {
		for (int row = 0; row < m_size; row++) {
			for (int col = 0; col < m_size; col++) {
				set(row, col, get(row, col) * scalar);
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
		return *this = (*this) * rhs;
	}

	friend Matrix<T> operator*(const Matrix<T>& lhs, const Matrix<T>& rhs) {
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

	friend Matrix<T> strassen3(const Matrix<T>& lhs, const Matrix<T>& rhs) {
		// when matrices degenerated to scalar (m_size = 1) just "normal" multiplication
		if (lhs.m_size == 1) {
			Matrix<T> result(lhs.m_padding, 1);
			result.set(0, 0, lhs.get(0, 0) * rhs.get(0, 0));
			return result;
		}

		// divide matrices to 9 (3x3) submatrices
		auto A = lhs.partition(3);
		const auto& A11 = A.get(0, 0);
		const auto& A12 = A.get(0, 1);
		const auto& A13 = A.get(0, 2);
		const auto& A21 = A.get(1, 0);
		const auto& A22 = A.get(1, 1);
		const auto& A23 = A.get(1, 2);
		const auto& A31 = A.get(2, 0);
		const auto& A32 = A.get(2, 1);
		const auto& A33 = A.get(2, 2);

		auto B = rhs.partition(3);
		const auto& B11 = B.get(0, 0);
		const auto& B12 = B.get(0, 1);
		const auto& B13 = B.get(0, 2);
		const auto& B21 = B.get(1, 0);
		const auto& B22 = B.get(1, 1);
		const auto& B23 = B.get(1, 2);
		const auto& B31 = B.get(2, 0);
		const auto& B32 = B.get(2, 1);
		const auto& B33 = B.get(2, 2);

		// calculate M_i submatrices (23 multiplications)
		auto M1 = strassen3(A11 + A12 + A13 - A21 - A22 - A32 - A33, B22);
		auto M2 = strassen3(A11 - A21, -B12 + B22);
		auto M3 = strassen3(A22, -B11 + B12 + B21 - B22 - B23 - B31 + B33);
		auto M4 = strassen3(-A11 + A21 + A22, B11 - B12 + B22);
		auto M5 = strassen3(A21 + A22, -B11 + B12);
		auto M6 = strassen3(A11, B11);
		auto M7 = strassen3(-A11 + A31 + A32, B11 - B13 + B23);
		auto M8 = strassen3(-A11 + A31, B13 - B23);
		auto M9 = strassen3(A31 + A32, -B11 + B13);
		auto M10 = strassen3(A11 + A12 + A13 - A22 - A23 - A31 - A32, B23);
		auto M11 = strassen3(A32, -B11 + B13 + B21 - B22 - B23 - B31 + B32);
		auto M12 = strassen3(-A13 + A32 + A33, B22 + B31 - B32);
		auto M13 = strassen3(A13 - A33, B22 - B32);
		auto M14 = strassen3(A13, B31);
		auto M15 = strassen3(A32 + A33, -B31 + B32);
		auto M16 = strassen3(-A13 + A22 + A23, B23 + B31 - B33);
		auto M17 = strassen3(A13 - A23, B23 - B33);
		auto M18 = strassen3(A22 + A23, -B31 + B33);
		auto M19 = strassen3(A12, B21);
		auto M20 = strassen3(A23, B32);
		auto M21 = strassen3(A21, B13);
		auto M22 = strassen3(A31, B12);
		auto M23 = strassen3(A33, B33);

		// calculated C_ij submatrices
		Matrix<T> result(lhs.m_padding, lhs.m_size);

        auto C = result.partition(3);
        C.set(0, 0, M6 + M14 + M19);
        C.set(0, 1, M1 + M4 + M5 + M6 + M12 + M14 + M15);
        C.set(0, 2, M6 + M7 + M9 + M10 + M14 + M16 + M18);
        C.set(1, 0, M2 + M3 + M4 + M6 + M14 + M16 + M17);
        C.set(1, 1, M2 + M4 + M5 + M6 + M20);
        C.set(1, 2, M14 + M16 + M17 + M18 + M21);
        C.set(2, 0, M6 + M7 + M8 + M11 + M12 + M13 + M14);
        C.set(2, 1, M12 + M13 + M14 + M15 + M22);
        C.set(2, 2, M6 + M7 + M8 + M9 + M23);

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
};

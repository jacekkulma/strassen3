#pragma once

#include <iostream>
#include <vector>
#include <cstring>

template<class T>
class Matrix {
public:
	Matrix(T padding) : Matrix(nullptr, padding, 0, 0, 0, 0, 0, 0)
	{ }

	Matrix() : Matrix(nullptr, T{}, 0, 0, 0, 0, 0, 0) {}

	Matrix(const Matrix<T>& matrix) 
	{
		*this = matrix;
	}

	Matrix(int size)
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
		m_padding = matrix.m_padding;
		m_dataSize = matrix.m_dataSize;
		m_isDataOwner = matrix.m_isDataOwner;
		m_rowsStart = matrix.m_rowsStart;
		m_colsStart = matrix.m_colsStart;
		m_rowsEnd = matrix.m_rowsEnd;
		m_colsEnd = matrix.m_colsEnd;
		m_size = matrix.m_size;

		if (!matrix.m_isDataOwner) {
			m_data = matrix.m_data;
			return *this;
		}

		m_data = new T[m_dataSize * m_dataSize];
		std::memcpy(m_data, matrix.m_data, m_dataSize * m_dataSize * sizeof(T));

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
	
	T get(int row, int col) const
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

	Matrix<Matrix<T>> partition(int count) const
	{
		int partitionedSize = (m_size + count - 1) / count;
		Matrix<Matrix<T>> matrix(count);
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
				matrix.set(row, col, submatrix);
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
		Matrix<T> result(m_padding);
		result.resize(m_size);

		for (int i = 0; i < m_size; i++) {
			for (int j = 0; j < m_size; j++) {
				T sum = 0;
				for (int k = 0; k < m_size; k++) {
					sum += get(i, k) * rhs.get(k, j);
				}
				result.set(i, j, sum);
			}
		}
		*this = result;
		return *this;
	}

	friend Matrix<T> operator*(const Matrix<T>& lhs, const Matrix<T>& rhs) {
		Matrix<T> result(lhs.m_padding);
		result.resize(lhs.m_size);

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

	Matrix<T> strassen3(const Matrix<T>& other) const {
		// when matrices degenerated to scalar (m_size = 1) just "normal" multiplication
		if (m_size == 1) {
			Matrix<T> result(m_padding);
			result.resize(1);
			result.set(0, 0, get(0, 0) * other.get(0, 0));
			return result;
		}

		// divide matrices to 9 (3x3) submatrices
		Matrix<Matrix<T>> A, B;
		/*A.resize(m_size);
		B.resize(m_size);*/
		A = this->partition(3);
		B = other.partition(3);

		//return A.get(0, 0);
		Matrix<T> A11, A12, A13, A21, A22, A23, A31, A32, A33;
		A11.resize(m_size / 3);
		A12.resize(m_size / 3);
		A13.resize(m_size / 3);
		A21.resize(m_size / 3);
		A22.resize(m_size / 3);
		A23.resize(m_size / 3);
		A31.resize(m_size / 3);
		A32.resize(m_size / 3);
		A33.resize(m_size / 3);

		A11 = A.get(0, 0);
		A12 = A.get(0, 1);
		A13 = A.get(0, 2);
		A21 = A.get(1, 0);
		A22 = A.get(1, 1);
		A23 = A.get(1, 2);
		A31 = A.get(2, 0);
		A32 = A.get(2, 1);
		A33 = A.get(2, 2);

		auto B11 = B.get(0, 0);
		auto B12 = B.get(0, 1);
		auto B13 = B.get(0, 2);
		auto B21 = B.get(1, 0);
		auto B22 = B.get(1, 1);
		auto B23 = B.get(1, 2);
		auto B31 = B.get(2, 0);
		auto B32 = B.get(2, 1);
		auto B33 = B.get(2, 2);

		// calculate M_i submatrices (23 multiplications)
		auto M1 = (A11 + A12 + A13 - A21 - A22 - A32 - A33).strassen3(B22);
		auto M2 = (A11 - A21).strassen3(-B12 + B22);
		auto M3 = A22.strassen3(-B11 + B12 + B21 - B22 - B23 - B31 + B33);
		auto M4 = (-A11 + A21 + A22).strassen3(B11 - B12 + B22);
		auto M5 = (A21 + A22).strassen3(-B11 + B12);
		auto M6 = A11.strassen3(B11);
		auto M7 = (-A11 + A31 + A32).strassen3(B11 - B13 + B23);
		auto M8 = (-A11 + A31).strassen3(B13 - B23);
		auto M9 = (A31 + A32).strassen3(-B11 + B13);
		auto M10 = (A11 + A12 + A13 - A22 - A23 - A31 - A32).strassen3(B23);
		auto M11 = A32.strassen3(-B11 + B13 + B21 - B22 - B23 - B31 + B32);
		auto M12 = (-A13 + A32 + A33).strassen3(B22 + B31 - B32);
		auto M13 = (A13 - A33).strassen3(B22 - B32);
		auto M14 = A13.strassen3(B31);
		auto M15 = (A32 + A33).strassen3(-B31 + B32);
		auto M16 = (-A13 + A22 + A23).strassen3(B23 + B31 - B33);
		auto M17 = (A13 - A23).strassen3(B23 - B33);
		auto M18 = (A22 + A23).strassen3(-B31 + B33);
		auto M19 = A12.strassen3(B21);
		auto M20 = A23.strassen3(B32);
		auto M21 = A21.strassen3(B13);
		auto M22 = A31.strassen3(B12);
		auto M23 = A33.strassen3(B33);

		// calculated C_ij submatrices
		auto C11 = M6 + M14 + M19;
		auto C12 = M1 + M4 + M5 + M6 + M12 + M14 + M15;
		auto C13 = M6 + M7 + M9 + M10 + M14 + M16 + M18;
		auto C21 = M2 + M3 + M4 + M6 + M14 + M16 + M17;
		auto C22 = M2 + M4 + M5 + M6 + M20;
		auto C23 = M14 + M16 + M17 + M18 + M21;
		auto C31 = M6 + M7 + M8 + M11 + M12 + M13 + M14;
		auto C32 = M12 + M13 + M14 + M15 + M22;
		auto C33 = M6 + M7 + M8 + M9 + M23;

		// build result matrix
		Matrix<T> result(m_padding);
		result.resize(m_size);

		int blockSize = m_size / 3;

		for (int row = 0; row < blockSize; row++) {
			for (int col = 0; col < blockSize; col++) {
				result.set(row, col, C11.get(row, col));
				result.set(row, col + blockSize, C12.get(row, col));
				result.set(row, col + 2 * blockSize, C13.get(row, col));

				result.set(row + blockSize, col, C21.get(row, col));
				result.set(row + blockSize, col + blockSize, C22.get(row, col));
				result.set(row + blockSize, col + 2 * blockSize, C23.get(row, col));

				result.set(row + 2 * blockSize, col, C31.get(row, col));
				result.set(row + 2 * blockSize, col + blockSize, C32.get(row, col));
				result.set(row + 2 * blockSize, col + 2 * blockSize, C33.get(row, col));
			}
		}

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
		// TODO
		matrix.resize(3);

		for (int row = 0; row < matrix.m_size; row++) {
			for (int col = 0; col < matrix.m_size; col++) {
				T value{};
				is >> value;
				matrix.set(row, col, value);
			}
		}

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

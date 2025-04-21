#pragma once

#include <iostream>
#include <vector>
#include <cstring>

template<class T>
class Matrix {
public:
	Matrix(T padding) : Matrix(nullptr, padding, 0, 0, 0, 0, 0, 0)
	{ }

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

	Matrix<Matrix<T>> partition(int count)
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

#include "Matrix.h"
#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;
Matrix::Matrix()
{
	rowNum = 0;
	colNum = 0;
	item = NULL;
}
//零矩阵
Matrix::Matrix(int m, int n)
{
	if (m < 0 || n < 0)
	{
		cout << "矩阵大小不能为负\n";
		return;
	}
	rowNum = m;
	colNum = n;
	item = new double[m*n];
	for (int i = 0; i < m*n; i++)
	{
		item[i] = 0;
	}
}

//也可用二维数组初始化
Matrix::Matrix(double* items, int m, int n)
{
	rowNum = m;
	colNum = n;
	item = new double[m*n];
	for (int i = 0; i < colNum*rowNum; i++)
	{
		item[i] = items[i];
	}
}
//单位矩阵
Matrix::Matrix(int n)
{
	rowNum = colNum = n;
	item = new double[n*n];
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (i == j)
				set(i, j, 1);
			else
				set(i, j, 0);
		}
	}
}
Matrix::Matrix(const Matrix &M)
{
	colNum = M.colNum;
	rowNum = M.rowNum;
	//这里不能对指针直接赋值,复制对求逆、转置等操作会影响原矩阵
	item = new double[colNum*rowNum];
	for (int i = 0; i < colNum*rowNum; i++)
	{
		item[i] = M.item[i];
	}
}
Matrix& Matrix::operator=(const Matrix &M)
{
	colNum = M.colNum;
	rowNum = M.rowNum;
	if (item != NULL) delete[] item;
	item = new double[colNum*rowNum];
	for (int i = 0; i < colNum*rowNum; i++)
	{
		this->item[i] = M.item[i];
	}
	return *this;
}
Matrix::~Matrix()
{
	delete[] item;
}
double Matrix::get(int i, int j) const
{
	return item[i*colNum + j];
}
void Matrix::set(int i, int j, double value)
{
	item[i*colNum + j] = value;
}
void Matrix::RowSwap(int i, int j, double multiply)
{
	if (j == -1)
	{
		for (int k = 0; k < colNum; k++)
		{
			set(i, k, multiply*get(i, k));
		}
	}
	else
	{
		for (int k = 0; k < colNum; k++)
		{
			set(j, k, multiply*get(i, k) + get(j, k));
		}
	}
}
void Matrix::RowSwap(int i, int j)
{
	Matrix _copy = *this;
	for (int k = 0; k < colNum; k++)
	{
		double swap = _copy.get(j, k);
		set(j, k, _copy.get(i, k));
		set(i, k, swap);
	}
}
Matrix Matrix::Trans() const
{
	Matrix _copy (this->colNum,this->rowNum);
	for (int i = 0; i <this->rowNum; i++)
	{
		for (int j = 0; j <this->colNum; j++)
		{
			_copy.set(j, i, this->get(i, j));
		}
	}
	//_copy.rowNum = this->colNum;
	//_copy.colNum = this->rowNum;
	return _copy;
}
int Matrix::getRowNum() const
{
	return rowNum;
}
int Matrix::getColNum() const
{
	return colNum;
}
ostream& operator <<(ostream &os, const Matrix &m)
{
	for (int i = 0; i < m.rowNum; i++)
	{
		for (int j = 0; j < m.colNum; j++)
		{
			os <<std::right<<setw(7)<<setprecision(4)<< m.get(i, j);
			if (j == m.colNum - 1)  os << "\n";
		}
	}
	return os;
}
Matrix Matrix::operator*(const double f)
{
	Matrix _copy = *this;
	for (int i = 0; i <rowNum ; i++)
	{
		for (int j = 0; j < colNum; j++)
		{
			_copy.set(i, j, get(i, j)*f);
		}
	}
	return _copy;
}
Matrix Matrix::operator-()
{
	Matrix _copy = *this;
	for (int i = 0;i < colNum*rowNum;i++)
		_copy.item[i] = -item[i];
	return _copy;
}
Matrix operator*(const double f,const Matrix & m)
{
	Matrix _copy = m;
	for (int i = 0; i < m.getRowNum(); i++)
	{
		for (int j = 0; j < m.getColNum(); j++)
		{
			_copy.set(i, j, m.get(i, j)*f);
		}
	}
	return _copy;
}
Matrix Matrix::operator +(const Matrix &m)
{
	if (m.colNum != colNum || m.rowNum != rowNum) return *this;
	Matrix _copy = *this;
	for (int i = 0; i < rowNum; i++)
	{
		for (int j = 0; j < colNum; j++)
		{
			_copy.set(i, j, get(i, j) + m.get(i, j));
		}
	}
	return _copy;
}
Matrix Matrix::operator -(const Matrix &m)
{
	if (m.colNum != colNum || m.rowNum != rowNum) return *this;
	Matrix _copy = *this;
	for (int i = 0; i < rowNum; i++)
	{
		for (int j = 0; j < colNum; j++)
		{
			_copy.set(i, j, get(i, j) - m.get(i, j));
		}
	}
	return _copy;
}

Matrix Matrix::operator *(const Matrix &m)
{
	if (colNum != m.rowNum)
	{
		cout << "无法相乘！";
		return *this;
	}
	Matrix _copy(this->rowNum, m.colNum);
	for (int i = 0; i < this->rowNum; i++)
	{
		for (int j = 0; j < m.colNum; j++)
		{
			double sum = 0;
			for (int k = 0; k < m.rowNum; k++)
			{
				sum += this->get(i, k)*m.get(k, j);
			}
			_copy.set(i, j, sum);
		}
	}
	return _copy;
}
double Matrix::operator /(const double f)
{
	double _copy=0;
	if (rowNum == 1 && colNum == 1)
	{
		_copy = item[0] / f;
		return _copy;
	}
	else cout << "矩阵行数和列数不为一"<<endl;
	return _copy;
}
Matrix Matrix::Inverse()
{
	Matrix _copy = *this;
	Matrix result(colNum);
	Matrix error(colNum,colNum);
	if (colNum != rowNum)
	{
		cout << "矩阵不可逆！" << endl;
		return *this;
	}
	for (int i = 0; i < colNum; i++)
	{
		int MaxRow = i;
		double max = fabs(_copy.get(i, i));
		for (int j = i; j < rowNum; j++)
		{
			if (fabs(_copy.get(j, i)) > max)
			{
				max = fabs(_copy.get(j, i));
				MaxRow = j;
			}
		}
		if (max == 0)
		{
			cout << "不是满秩矩阵";
			return error;
		}
		if (MaxRow != i)
		{
			result.RowSwap(i, MaxRow);
			_copy.RowSwap(i, MaxRow);
		}
		double r = 1.0 / _copy.get(i, i);
		//单位化
		_copy.RowSwap(i, -1, r);
		result.RowSwap(i, -1, r);
		for (int j = 0; j < rowNum; j++)
		{
			if (j == i) continue;
			r = -_copy.get(j, i);
			_copy.RowSwap(i, j, r);
			result.RowSwap(i, j, r);
		}
	}
	return result;
}

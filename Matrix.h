#pragma once
#include <iostream>
class Matrix
{
public:
	Matrix();
	Matrix(int m, int n);			 //构建一个m*n的全零矩阵
	Matrix(int n);				 //构建一个n*n的单位矩阵
	Matrix(const Matrix &);		 //拷贝构造函数，深拷贝
	Matrix(double* items, int m, int n);//根据数组拷贝一个矩阵
	~Matrix();

	int getRowNum() const;				//返回矩阵的行数
	int getColNum() const;				//返回矩阵的列数

	Matrix Trans() const;				//将矩阵转置
	//矩阵初等行变换
	//如果j=-1,则对i扩大multiply倍
	//如果j在取值范围内，则将第i行扩大multiply倍加到j行
	void RowSwap(int i, int j, double multiply);
	//交换两行
	void RowSwap(int i, int j);
	double get(int i, int j) const;			//返回矩阵第i行j列元素
	void set(int i, int j, double val);		//设置矩阵第i行j列元素


	Matrix operator +(const Matrix &m);	 	//两个矩阵相加
	Matrix operator -(const Matrix &m);	    //两个矩阵相减
	Matrix operator *(const Matrix &m);     //两个矩阵相乘
	double operator /(const double f);      //当矩阵为一行一列时除以常数
	Matrix& operator=(const Matrix &m);
	Matrix operator -();                    //对矩阵取负
	Matrix Inverse();
	friend std::ostream& operator <<(std::ostream &os, const Matrix &m);//简单重载<<
    Matrix operator *(const double f);         //矩阵乘以常数，常数在后
	friend Matrix operator *(const double f,const Matrix &m);    //矩阵乘以常数，常数在前
private:
	double *item;		//指向矩阵首元素的指针
	int rowNum;		//矩阵行数
	int colNum;		//矩阵列数
};
#pragma once
#include "DATA.h"

class COLUMN
{ //一列数据
private:
	std::vector<pDATA> ColumnData;           //一列数据
	bool DefaultFlag = false;                //是否有默认值：true->有default值
	pDATA Default;                           //默认值
	std::vector<ConstraintType> Constraints; //限制的集合
	std::string extra;                       //EXTRA
public:
	void del(int k);                       //删除第k+1个数据
	void update(int k, int v);             //更新第k+1个数据为v
	void update(int k, std::string v);     //更新第k+1个数据为v
	void update(int k, double v);          //更新第k+1个数据为v
	bool Get_IsNull(int k);                //判断第k+1个数据是否为NULL
	int Get_INT_Value(int k);              //获取第k+1个值
	double Get_DOUBLE_Value(int k);        //获取第k+1个值
	std::string Get_CHAR_Value(int k);            //获取第k+1个值
	void push_back(int d);                 //直接加入数据
	void push_back(std::string d);                //直接加入数据
	void push_back(double d);              //直接加入数据
	void push_back_null(double d);         //直接加入NULL数据
	void push_back_null(int d);            //直接加入NULL数据
	void push_back_null(std::string d);           //直接加入NULL数据
	void SetConstraint(ConstraintType ct); //添加限制
	bool isNotNULL();                      //判断有无NOT_NULL限制
	bool hasDefault();                     //判断有无默认值
	pDATA GetDefault();                    //获取默认值
	void SetDefault(int d);                //设置默认值
	void SetDefault(std::string d);        //设置默认值
	void SetDefault(double d);             //设置默认值
	std::string GetExtra();                //获取extra
	int size()
	{ //获得当前列数据的个数（行数）
		return (int)ColumnData.size();
	}
	~COLUMN()
	{ //析构函数
		for (auto it = ColumnData.begin(); it < ColumnData.end(); it++)
		{
			if (*it != NULL)
				delete *it;
		}
	}

	//以下四个函数返回派生类对象指针
	INT *Get_INT_Data(int k);
	DOUBLE *Get_DOUBLE_Data(int k);
	CHAR *Get_CHAR_Data(int k);
};


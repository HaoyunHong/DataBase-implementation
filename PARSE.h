#pragma once
#include "ALLBASES.h"

class PARSE
{
private:
	DATABASE *curDb;	   //当前操作的数据库
	TABLE *curTb;		   //当前操作的表
	std::string curdbname; //当前操作的数据库名
public:
	void EXEC(ALLBASES &Allbases, std::string input);   //输入命令处理
	void Transform(std::string &src, std::string &dst); //关键字转换为大写
	DataType attype(std::string type);					//获得数据类型
	std::string whole_expression_standardize(std::string whole_expression);//对缺乏括号的算数符号进行处理
	int P(std::string arithmetic_operator);//制定算术运算符优先级
	std::string arithmetic_calculator(std::string& whole_expression);//算数计算器
	int PL(std::string logic_operator);//制定逻辑运算符优先级
	std::string logic_calculator(std::string& whole_expression);//逻辑计算器
	std::string constify(std::string condition, const std::vector<std::string> &tbname, TABLE *ptb, int line_num, const std::vector<int>outorder, const std::vector<std::string> &col_name, const std::vector<std::string> &col_tb_name);//将一个列内的相关数据代换成常数
};



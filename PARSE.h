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
};

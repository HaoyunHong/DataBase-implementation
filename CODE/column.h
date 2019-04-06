#pragma once
#include <iostream>
#include <vector>
#include <string>

class COLUMN //抽象类
{
  private:
	std::string m_name;
	bool m_is_primary; //判断是不是主键
	int m_type;		   //0=int， 1=double， 2=字符串

  public:
	COLUMN(std::string s, int i);
	virtual ~COLUMN() = 0; //纯虚函数
	std::string &get_name();
	bool get_is_primary();
	void set_is_primary();
	int get_type();
};
class INT_COLUMN : public COLUMN
{
  private:
	std::vector<int> m_value;

  public:
	INT_COLUMN(std::string s);
	std::vector<int> &get_value();
};
class DOUBLE_COLUMN : public COLUMN
{
  private:
	std::vector<double> m_value;

  public:
	DOUBLE_COLUMN(std::string s);
	std::vector<double> &get_value();
};
class CHAR_COLUMN : public COLUMN
{
  private:
	std::vector<std::string> m_value;

  public:
	CHAR_COLUMN(std::string s);
	std::vector<std::string> &get_value();
};
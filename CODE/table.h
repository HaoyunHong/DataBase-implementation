#pragma once
#include "column.h"
#include <iostream>
#include <vector>
#include <string>

class TABLE
{
  private:
	std::vector<COLUMN *> m_column; //只有这个vector是指针！
	std::string m_name;

  public:
	COLUMN *operator[](const int &k);
	std::vector<COLUMN *> &get_column();
	std::string& get_name();
	COLUMN *search_column(const std::string& s);
	TABLE(std::string s);
	void show_columns();
};
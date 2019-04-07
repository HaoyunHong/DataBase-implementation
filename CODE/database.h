#pragma once
#include "table.h"
#include <iostream>
#include <vector>
#include <string>

class DATABASE
{
  private:
	std::vector<TABLE> m_table;
	std::string m_name;

  public:
	TABLE &operator[](const int &k);
	std::vector<TABLE> &get_table();
	std::string& get_name();
	DATABASE(std::string name);
};
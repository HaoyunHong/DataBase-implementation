#pragma once
#include "table.h"
#include <iostream>
#include <vector>
#include <cstring>
class DATABASE
{
  private:
    std::vector<TABLE> m_table;
    std::string m_name;

  public:
    TABLE &operator[](const int &k);
    std::vector<TABLE> &get_table();
};
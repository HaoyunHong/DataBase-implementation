#pragma once
#include "column.h"
#include <iostream>
#include <vector>
#include <cstring>
class TABLE
{
  private:
    std::vector<COLUMN*> m_column;//只有这个vector是指针！
    std::string m_name;

  public:
    COLUMN *operator[](const int &k);
    std::vector<COLUMN*>&get_column();
};
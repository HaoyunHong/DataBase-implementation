#pragma once
#include "database.h"
#include <iostream>
#include <vector>
#include <cstring>
class SYSTEM
{
  private:
    std::vector<DATABASE> m_database;

  public:
    DATABASE &operator[](const int &k);
    std::vector<DATABASE> &get_database();
};

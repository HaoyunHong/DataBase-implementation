#pragma once
#include <iostream>
#include <vector>
#include <cstring>
class COLUMN//抽象类
{
  private:
    std::string m_name;
    bool m_is_primary; //判断是不是主键
    int m_type;//0=int， 1=double， 2=字符串
  public:
    virtual ~COLUMN()=0;
};
class INT_COLUMN:public COLUMN
{

};
class DOUBLE_COLUMN:public COLUMN
{
    
};
class CHAR_COLUMN:public COLUMN
{

};
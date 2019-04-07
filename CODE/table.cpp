#include "table.h"

COLUMN *TABLE::operator[](const int &k)
{
    return m_column[k];
}
std::vector<COLUMN *> &TABLE::get_column() { return m_column; }
std::string &TABLE::get_name() { return m_name; }

TABLE::TABLE(std::string s):m_name(s){}
//传入名字的构造函数

COLUMN *TABLE::search_column(std::string s)
{
    int total = m_column.size();
    for(int i = 0; i < total; i++)
    {
        if(m_column[i]->get_name() == s)
        return m_column[i];
    }
}//寻找列，返回一个指向该列的指针
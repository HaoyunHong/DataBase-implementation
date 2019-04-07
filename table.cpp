#include "table.h"

COLUMN *TABLE::operator[](const int &k)
{
    return m_column[k];
}
std::vector<COLUMN *> &TABLE::get_column() { return m_column; }
std::string &TABLE::get_name() { return m_name; }

TABLE::TABLE(std::string s) : m_name(s) {}
//传入名字的构造函数F

COLUMN *TABLE::search_column(const std::string &s)
{
    int total = m_column.size();
    for (int i = 0; i < total; i++)
    {
        if (m_column[i]->get_name() == s)
            return m_column[i];
    }
} //寻找列，返回一个指向该列的指针
void TABLE::show_columns()
{
    if(m_column.size() == 0)
    {
        printf("%s\n", "There aren't any columns!");
        return;
    }
    printf("%s\t%s\t%s\n", "NAME", "TYPE", "PRIMARY_KEY");
    for (int i = 0; i < m_column.size(); i++)
    {
        printf("%s\t%s\t", m_column[i]->get_name(), m_column[i]->get_type());
        if (m_column[i]->get_is_primary())
            printf("%s\n", "YES");
        else
            printf("%s\n", "NO");
    }
}//输出所有列的信息
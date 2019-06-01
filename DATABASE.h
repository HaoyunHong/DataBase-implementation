#pragma once
#include "TABLE.h"

class DATABASE
{ //一个数据库
private:
    std::vector<std::string> TableName; //表名的集合
public:
    std::map<std::string, TABLE *> DataBaseMap; //表名->表指针的映射

    TABLE *GetTable(std::string name)
    { //获得名字为name的表的表指针
        auto it = DataBaseMap.find(name);
        if (it == DataBaseMap.end())
        {
            std::cout << "Can't find " << name << std::endl;
            return nullptr;
        }
        else
        {
            return it->second;
        }
    }

    void addtable(std::string tbname, TABLE *ptable)
    { //添加表；参数（表名， 表指针）
        TableName.push_back(tbname);
        if (ptable != NULL)
        {
            DataBaseMap.insert(make_pair(tbname, ptable));
        }
    }

    void deltable(std::string tbname)
    { //删除表；参数（表名）
        for (auto it = TableName.begin(); it != TableName.end(); it++)
        {
            if (*it == tbname)
            {
                TableName.erase(it);
                if (DataBaseMap[tbname] != NULL)
                {
                    delete DataBaseMap[tbname];
                }
                DataBaseMap.erase(tbname);
                break;
            }
        }
    }

    void showtables()
    {                                             //直接列出现有表
        sort(TableName.begin(), TableName.end()); //按字典序
        for (auto it = TableName.begin(); it != TableName.end(); it++)
        {
            std::cout << *it << std::endl;
        }
    }
};

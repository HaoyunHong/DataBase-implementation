#pragma once
#include "TABLE.h"
#include <iostream>
#include <fstream>

class DATABASE
{ //一个数据库
private:
    std::string name;
    std::vector<std::string> TableName; //表名的集合
public:
    std::map<std::string, TABLE *> DataBaseMap;    //表名->表指针的映射
    DATABASE(const std::string &a) : name(a) {}    //利用名字构造
    const std::string &get_name() { return name; } //获取数据库的名字
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
    void create_database_file()
    {
        std::ofstream fout;
        std::string filename = "./data/" + name + ".txt";
        fout.open(filename);
        fout << TableName.size() << "\n";
        for (int i = 0; i < TableName.size(); i++)
            fout << TableName[i] << "\n";
        fout.flush();
        fout.close();
        for (int i = 0; i < TableName.size(); i++)
        {
            DataBaseMap[TableName[i]]->create_table_file(name);
        }
    }
    void load_database()
    {
        std::fstream fst;
        std::string filename = "./data/" + name + ".txt";
        fst.open(filename);
        int table_num;
        fst >> table_num;
        std::string tablename;
        fst.get();
        for (int i = 0; i < table_num; i++)
        {
            getline(fst, tablename);
            addtable(tablename, new TABLE(tablename));
        }
        fst.close();
        for (int i = 0; i < TableName.size(); i++)
            DataBaseMap[TableName[i]]->load_table(name);
    }
};

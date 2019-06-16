#include "ALLBASES.h"
#include <iostream>
#include <fstream>
using namespace std;

DATABASE *ALLBASES::GetDB(string name)
{ //获得数据库指针
	auto it = AllBasesMap.find(name);
	if (it != AllBasesMap.end())
		return it->second;
	else
		cout << "Can't find " << name << endl;
	return nullptr;
}

void ALLBASES::create(string dbname)
{ //建立新数据库
	DBName.push_back(dbname);
	AllBasesMap.insert(make_pair(dbname, new DATABASE(dbname)));
	return;
}

void ALLBASES::del(string name)
{ //Drop数据库
	for (auto it = DBName.begin(); it != DBName.end(); it++)
	{
		if (*it == name)
		{
			DBName.erase(it);
			if (AllBasesMap[name] != NULL)
			{
				delete AllBasesMap[name]; //delete DATABASE*
			}
			AllBasesMap.erase(name);
			break;
		}
	}
	return;
}

void ALLBASES::show()
{ //列出当前数据库,按字典序
	cout << "Database" << endl;
	sort(DBName.begin(), DBName.end());
	for (auto it = DBName.begin(); it != DBName.end(); it++)
	{
		std::cout << *it << std::endl;
	}
	return;
}

const std::vector<std::string> &ALLBASES::Get_DBName()
{
	return DBName;
}

void ALLBASES::create_allbases_file() //存档，将allbases类里面每个数据库的名字存到一个文件里面去
{
	std::string filename = "./data/" + std::string("index.txt");
	ofstream fst(filename);
	int h = DBName.size();
	fst << h;
	fst << "\n";
	for (int i = 0; i < DBName.size(); i++)
		fst << DBName[i] << "\n";
	fst.flush();
	fst.close();

	for (int i = 0; i < DBName.size(); i++)
	{
		AllBasesMap[DBName[i]]->create_database_file();
	}
}

void ALLBASES::load_all_databases()
{
	fstream fst;
	string filename = "./data/" + string("index.txt");
	fst.open(filename); //默认把总管文件命名为index.txt
	int database_num;
	fst >> database_num;
	std::string dbname;//反复输入这一个字符串遍历所有数据库名
	fst.get();
	for(int i = 0; i < database_num; i++)
	{
		getline(fst, dbname);
		create(dbname);   //建立名为dbname的新数据库
	}
	fst.close();
	for(int i = 0; i < DBName.size(); i++)
		AllBasesMap[DBName[i]]->load_database();
} 

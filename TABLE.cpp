#include "TABLE.h"
#include <fstream>
using namespace std;

void split(const string &s, vector<string> &sv, const char flag)
{
	sv.clear();
	istringstream iss(s);
	string temp;
	while (getline(iss, temp, flag))
	{
		sv.push_back(temp);
	}
	return;
}

DataType TABLE::GetType(std::string name)
{
	for (int i = 0; i < ColumnName.size(); i++)
	{
		if (name == ColumnName[i])
		{
			return ColumnType[i];
		}
	}
	return _INT;
}

void TABLE::UpdateRow()
{ //更新行数
	for (auto it = TableMap.begin(); it != TableMap.end(); it++)
	{
		if (it->first == KeyColumn)
		{
			RowNum = it->second->size();
		}
	}
	return;
}

void TABLE::Update(string aname, int avalue, string condition)
{ //主要接口
	//（列名，目标值，条件语句）修改符合条件语句的行，使这些行的对应列的值为目标值
	auto it = TableMap.find(aname);
	if (it == TableMap.end())
	{
		cout << "Cant not find" << aname << endl;
		return;
	}
	this->UpdateRow();
	for (int i = 0; i < RowNum; i++)
	{
		if (this->Judge(condition, i))
		{
			it->second->update(i, avalue);
		}
	}
	return;
}

void TABLE::Update(string aname, char avalue, string condition)
{ //主要接口
	//（列名，目标值，条件语句）修改符合条件语句的行，使这些行的对应列的值为目标值
	auto it = TableMap.find(aname);
	if (it == TableMap.end())
	{
		cout << "Cant not find" << aname << endl;
		return;
	}
	this->UpdateRow();
	for (int i = 0; i < RowNum; i++)
	{
		if (this->Judge(condition, i))
		{
			it->second->update(i, avalue);
		}
	}
	return;
}

void TABLE::Update(string aname, double avalue, string condition)
{ //主要接口
	//（列名，目标值，条件语句）修改符合条件语句的行，使这些行的对应列的值为目标值
	auto it = TableMap.find(aname);
	if (it == TableMap.end())
	{
		cout << "Cant not find" << aname << endl;
		return;
	}
	this->UpdateRow();
	for (int i = 0; i < RowNum; i++)
	{
		if (this->Judge(condition, i))
		{
			it->second->update(i, avalue);
		}
	}
	return;
}

bool TABLE::MyCompare(int a, int b, string cname)
{ //由角标对cname列大小作比较
	for (auto it = TableMap.begin(); it != TableMap.end(); it++)
	{
		if (it->first == cname)
		{
			DataType type = this->GetType(cname);
			if (type == _INT)
			{
				return it->second->Get_INT_Value(a) < it->second->Get_INT_Value(b);
			}
			else if (type == _CHAR)
			{
				return it->second->Get_CHAR_Value(a) < it->second->Get_CHAR_Value(b);
			}
			else if (type == _DOUBLE)
			{
				return it->second->Get_DOUBLE_Value(a) < it->second->Get_DOUBLE_Value(b);
			}
		}
	}
	return false;
}

/*void TABLE::Select(string name, string condition)
{ //主要接口
	//查找某些行并输出
	this->UpdateRow();
	if (name == "*")
	{ //输出所有列
		if (RowNum == 0)
			return;
		vector<int> outorder; //输出顺序
		for (auto it = ColumnName.begin(); it < ColumnName.end(); ++it)
		{
			cout << *it << "\t";
		}
		cout << endl;
		for (int j = 0; j < RowNum; j++)
		{ //遍历所有行
			if (Judge(condition, j))
			{ //如果满足
				outorder.push_back(j);
			}
		}
		if (outorder.size() > 1)
		{ //冒泡排序，按照主键顺序
			for (int i = 0; i < (outorder.size() - 1); i++)
			{
				if (i >= (outorder.size() - 1))
					break;
				for (int j = 0; j < (outorder.size() - 1 - i); j++)
				{
					if (this->MyCompare(outorder[j + 1], outorder[j], KeyColumn))
					{
						int tmp = outorder[j + 1];
						outorder[j + 1] = outorder[j];
						outorder[j] = tmp;
					}
				}
			}
		}
		//输出
		for (int j = 0; j < outorder.size(); j++)
		{ //遍历outorder
			for (int i = 0; i < ColumnName.size(); i++)
			{ //遍历所有列
				if (ColumnType[i] == _INT)
				{ //判断列的类型
					auto tem = TableMap[ColumnName[i]];
					if (tem->Get_IsNull(outorder[j]))
						cout << "NULL\t";
					else
						cout << tem->Get_INT_Value(outorder[j]) << "\t";
				}
				else if (ColumnType[i] == _CHAR)
				{
					auto tem = TableMap[ColumnName[i]];
					if (tem->Get_IsNull(outorder[j]))
						cout << "NULL\t";
					else
						cout << tem->Get_CHAR_Value(outorder[j]) << "\t";
				}
				else if (ColumnType[i] == _DOUBLE)
				{
					auto tem = TableMap[ColumnName[i]];
					if (tem->Get_IsNull(outorder[j]))
						cout << "NULL\t";
					else
						cout << fixed << setprecision(4) << tem->Get_DOUBLE_Value(outorder[j]) << "\t";
				}
			}
			cout << endl;
		}
	}
	else
	{						  //输出某一列
		vector<int> outorder; //输出的行的行标顺序
		auto it = TableMap.find(name);
		if (it == TableMap.end())
		{
			cout << "Can not find " << name << endl;
			return;
		}
		DataType CType = _INT;
		for (int i = 0; i < ColumnName.size(); i++)
		{
			if (ColumnName[i] == it->first)
			{
				CType = ColumnType[i];
			}
		}
		for (int j = 0; j < RowNum; j++)
		{
			if (Judge(condition, j))
			{
				outorder.push_back(j);
			}
		}
		if (outorder.size() > 1)
		{ //冒泡排序
			for (int i = 0; i < (outorder.size() - 1); i++)
			{
				if (i >= (outorder.size() - 1))
					break;
				for (int j = 0; j < (outorder.size() - 1 - i); j++)
				{
					if (this->MyCompare(outorder[j + 1], outorder[j], KeyColumn))
					{
						int tmp = outorder[j + 1];
						outorder[j + 1] = outorder[j];
						outorder[j] = tmp;
					}
				}
			}
		}
		if (outorder.size() > 0)
		{ //输出
			cout << name << endl;
			auto pc = TableMap[name];
			auto type = GetType(name);
			if (type == _INT)
			{
				for (int j = 0; j < outorder.size(); j++)
				{
					if (pc->Get_IsNull(outorder[j]))
						cout << "NULL\t";
					else
						cout << pc->Get_INT_Value(outorder[j]) << endl;
					;
				}
			}
			else if (type == _CHAR)
			{
				for (int j = 0; j < outorder.size(); j++)
				{
					if (pc->Get_IsNull(outorder[j]))
						cout << "NULL\t";
					else
						cout << pc->Get_CHAR_Value(outorder[j]) << endl;
					;
				}
			}
			else if (type == _DOUBLE)
			{
				for (int j = 0; j < outorder.size(); j++)
				{
					if (pc->Get_IsNull(outorder[j]))
						cout << "NULL\t";
					else
						cout << pc->Get_DOUBLE_Value(outorder[j]) << endl;
					;
				}
			}
		}
	}
	return;
}*/

const std::vector<int> &TABLE::Select(const std::vector<std::string> &col_name, string condition)
{ //主要接口,其实这里col_name没有用到
	//查找某些行并输出
	static std::vector<int> outorder;
	this->UpdateRow();
	if (RowNum == 0)
		return outorder;
	outorder.clear();
	for (int j = 0; j < RowNum; j++)
	{ //遍历所有行
		if (Judge(condition, j))
		{ //如果满足
			outorder.push_back(j);
		}
	}
	if (outorder.size() > 1)
	{ //冒泡排序
		for (int i = 0; i < (outorder.size() - 1); i++)
		{
			if (i >= (outorder.size() - 1))
				break;
			for (int j = 0; j < (outorder.size() - 1 - i); j++)
			{
				if (this->MyCompare(outorder[j + 1], outorder[j], KeyColumn))
				{
					int tmp = outorder[j + 1];
					outorder[j + 1] = outorder[j];
					outorder[j] = tmp;
				}
			}
		}
	}
	return outorder;
}

void TABLE::show_output_from_select(const std::vector<std::string> &col_name, const std::vector<int> &outorder)
{
	this->UpdateRow();
	if (RowNum == 0)
		return;
	if (outorder.size() == 0)
		return; //如果不用输出，连表头都不需要打
	if (col_name[0] == "*")
	{
		for (int i = 0; i < ColumnName.size(); i++)
		{
			std::cout << ColumnName[i] << "\t";
		}
		std::cout << std::endl;
		for (int j = 0; j < outorder.size(); j++)
		{ //遍历outorder
			for (int i = 0; i < ColumnName.size(); i++)
			{ //遍历所有列
				if (ColumnType[i] == _INT)
				{ //判断列的类型
					auto tem = TableMap[ColumnName[i]];
					if (tem->Get_IsNull(outorder[j]))
						cout << "NULL\t";
					else
						cout << tem->Get_INT_Value(outorder[j]) << "\t";
				}
				else if (ColumnType[i] == _CHAR)
				{
					auto tem = TableMap[ColumnName[i]];
					if (tem->Get_IsNull(outorder[j]))
						cout << "NULL\t";
					else
						cout << tem->Get_CHAR_Value(outorder[j]) << "\t";
				}
				else if (ColumnType[i] == _DOUBLE)
				{
					auto tem = TableMap[ColumnName[i]];
					if (tem->Get_IsNull(outorder[j]))
						cout << "NULL\t";
					else
						cout << fixed << setprecision(4) << tem->Get_DOUBLE_Value(outorder[j]) << "\t";
				}
			}
			cout << endl;
		}
	}
	//在这里讨论如果“列名”是COUNT(expression)的情况,已知不会有WHERE语句
	else if (col_name[0].substr(0, 6) == "COUNT(" && col_name[0][col_name[0].length() - 1] == ')')
	{
		cout << col_name[0] << endl;
		string expression = col_name[0].substr(6, col_name[0].length() - 7);
		cout << this->Count(expression) << "\t";
	}
	else
	{
		if (outorder.size() > 0)
		{ //输出
			for (int p = 0; p < col_name.size(); p++)
			{
				cout << col_name[p] << "\t";
			}
			cout << endl;
			for (int j = 0; j < outorder.size(); j++)
			{
				for (int p = 0; p < col_name.size(); p++)
				{

					auto pc = TableMap[col_name[p]];
					auto type = GetType(col_name[p]);
					if (type == _INT)
					{
						if (pc->Get_IsNull(outorder[j]))
							cout << "NULL\t";
						else
							cout << pc->Get_INT_Value(outorder[j]) << "\t";
					}
					else if (type == _CHAR)
					{
						if (pc->Get_IsNull(outorder[j]))
							cout << "NULL\t";
						else
							cout << pc->Get_CHAR_Value(outorder[j]) << "\t";
					}
					else if (type == _DOUBLE)
					{
						if (pc->Get_IsNull(outorder[j]))
							cout << "NULL\t";
						else
							cout << pc->Get_DOUBLE_Value(outorder[j]) << "\t";
					}
				}
				cout << endl;
			}
		}
	}
}

void TABLE::Delete(string condition)
{ //主要接口
	//删除符合要求的行
	this->UpdateRow();
	int i = 0;
	while (1)
	{
		UpdateRow();
		if (i >= RowNum)
			break;
		if (Judge(condition, i))
		{
			this->Del(i);
			continue; //注意
		}
		++i;
	}
	return;
}

void TABLE::Del(int k)
{ //删除一行
	for (auto it = TableMap.begin(); it != TableMap.end(); it++)
	{
		it->second->del(k);
	}
	return;
}

COLUMN *TABLE::GetColumn(string name)
{ //Get COLUMN* by its name
	auto it = TableMap.find(name);
	if (it == TableMap.end())
	{
		cout << "Can't find " << name << endl;
		return nullptr;
	}
	else
	{
		return it->second;
	}
}

void TABLE::Insert(std::string cname, int d)
{ //在某列添加一条数据(列名，数据),需要三种重载
	for (auto it = TableMap.begin(); it != TableMap.end(); it++)
	{
		if (it->first == cname)
		{
			it->second->push_back(d);
		}
	}
	return;
}

void TABLE::Insert(std::string cname, char d)
{ //1 添加数据(列名，数据),需要三种重载
	for (auto it = TableMap.begin(); it != TableMap.end(); it++)
	{
		if (it->first == cname)
		{
			it->second->push_back(d);
		}
	}
	return;
}

void TABLE::Insert(std::string cname, double d)
{ //1 添加数据(列名，数据),需要三种重载
	for (auto it = TableMap.begin(); it != TableMap.end(); it++)
	{
		if (it->first == cname)
		{
			it->second->push_back(d);
		}
	}
	return;
}

void TABLE::InsertNull(string cname, DataType type)
{ //在某列添加一条为NULL的数据
	if (type == _INT)
	{
		int t = 0;
		for (auto it = TableMap.begin(); it != TableMap.end(); it++)
		{
			if (it->first == cname)
			{
				it->second->push_back_null(t);
			}
		}
	}
	else if (type == _CHAR)
	{
		char t = ' ';
		for (auto it = TableMap.begin(); it != TableMap.end(); it++)
		{
			if (it->first == cname)
			{
				it->second->push_back_null(t);
			}
		}
	}
	else if (type == _DOUBLE)
	{
		double t = 0.0;
		for (auto it = TableMap.begin(); it != TableMap.end(); it++)
		{
			if (it->first == cname)
			{
				it->second->push_back_null(t);
			}
		}
	}
	//对NULL的处理是对于每一个数据单元格都设置属性为NULL
}

void TABLE::addcolumn(std::string clname, DataType ctype)
{
	//添加列(列名，类型)
	ColumnName.push_back(clname);
	ColumnType.push_back(ctype);
	auto pc = new COLUMN;
	TableMap.insert(make_pair(clname, pc));
	return;
}

void TABLE::addcolumn(std::string clname, DataType ctype, vector<ConstraintType> &Constraints)
{
	//添加列(列名，类型，一个存放限制信息的vector)
	auto pc = new COLUMN;
	ColumnName.push_back(clname);
	ColumnType.push_back(ctype);
	TableMap.insert(make_pair(clname, pc));
	for (auto it = Constraints.begin(); it != Constraints.end(); it++)
	{
		pc->SetConstraint(*it);
	}
	return;
}

void TABLE::SetKey(string keyname)
{ //设置键值为keyname
	KeyColumn = keyname;
	return;
}

void TABLE::showcolumns()
{ //打印各列信息
	std::cout << "Field\tType\tNull\tKey\tDefault\tExtra\n";
	for (int i = 0; i < ColumnName.size(); i++)
	{
		auto pc = TableMap[ColumnName[i]];
		std::cout << ColumnName[i] << "\t";
		if (ColumnType[i] == _INT)
		{
			std::cout << "int(11)\t";
		}
		else if (ColumnType[i] == _DOUBLE)
		{
			std::cout << "double\t";
		}
		else if (ColumnType[i] == _CHAR)
		{
			std::cout << "char(1)\t";
		}
		if (pc->isNotNULL())
		{
			std::cout << "NO"
					  << "\t";
		}
		else
		{
			std::cout << "YES"
					  << "\t";
		}
		if (ColumnName[i] == KeyColumn)
		{
			std::cout << "PRI"
					  << "\t";
		}
		if (pc->hasDefault() == false)
		{
			std::cout << "NULL"
					  << "\t";
		}
		else
		{
			std::cout << pc->GetDefault() << "\t";
		}
		std::cout << pc->GetExtra() << std::endl;
	}
	return;
}

bool TABLE::Judge(string condition, int k)
{
	//功能：接收whereclause语句，一个整数行标（从0到行数-1），
	//输出bool,判断此行是否满足语句的条件
	if (condition == "true")
		return 1;

	//现分割逐句判断在运算逻辑运算式
	vector<string> Con; //存储> < = 语句
	vector<string> Sym; //存储逻辑运算符
	vector<bool> Res;   //存储每一句的bool值
	split(condition, Con);
	for (int i = 0; i < Con.size(); i++)
	{
		//遍历Con，若是< > = 语句则判断bool值，否则为逻辑运算符，加入到Sym并删除
		bool Null_Encountered = false; //如果symbol左边的列名所对应列值为空，直接跳过当前条件
		string ini = Con[i], upp;
		transform(ini.begin(), ini.end(), back_inserter(upp), ::toupper);
		if (upp == "AND" || upp == "OR")
		{
			if (upp == "AND")
			{
				Sym.push_back("AND");
			}
			if (upp == "OR")
			{
				Sym.push_back("OR");
			}
			Con.erase(Con.begin() + i);
			i--;
			continue;
		}
		//逐个遍历每一句中字符，若找到 > < = 则分割为两段，左为列名，将右边转化为对应列数据类型进行比较得到bool值
		for (int p = 0; p < Con[i].size(); p++)
		{
			if (Con[i][p] == '>' || Con[i][p] == '<' || Con[i][p] == '=')
			{
				char Symbol = Con[i][p];
				string value_name = Con[i].substr(0, p);
				string target_value = Con[i].substr(p + 1, Con[i].size());
				int index = -1;
				for (int j = 0; j < ColumnName.size(); j++)
				{
					if (value_name == ColumnName[j])
					{
						index = j;
						//如果对应列对应行中数据为空，当前条件直接判断为false
						if (TableMap[value_name]->Get_IsNull(k))
						{
							Null_Encountered = true;
							Res.push_back(false);
							break;
						}
					}
				}
				if (Null_Encountered)
					continue;
				if (index == -1)
				{
					cout << "error : colume name not found!" << endl; //for debug
				}
				DataType o = ColumnType[index];
				if (Symbol == '>')
				{

					if (o == _INT)
					{
						Res.push_back(TableMap[value_name]->Get_INT_Value(k) > stoi(target_value));
					}
					if (o == _DOUBLE)
					{
						Res.push_back(TableMap[value_name]->Get_DOUBLE_Value(k) > stod(target_value));
					}
					if (o == _CHAR)
					{
						Res.push_back(TableMap[value_name]->Get_CHAR_Value(k) > target_value[1]);
					}
				}
				if (Symbol == '<')
				{
					if (o == _INT)
					{
						Res.push_back(TableMap[value_name]->Get_INT_Value(k) < stoi(target_value));
					}
					if (o == _DOUBLE)
					{
						Res.push_back(TableMap[value_name]->Get_DOUBLE_Value(k) < stod(target_value));
					}
					if (o == _CHAR)
					{
						Res.push_back(TableMap[value_name]->Get_CHAR_Value(k) < target_value[1]);
					}
				}
				if (Symbol == '=')
				{
					if (o == _INT)
					{
						Res.push_back(TableMap[value_name]->Get_INT_Value(k) == stoi(target_value));
					}
					if (o == _DOUBLE)
					{
						Res.push_back(TableMap[value_name]->Get_DOUBLE_Value(k) == stod(target_value));
					}
					if (o == _CHAR)
					{
						Res.push_back(TableMap[value_name]->Get_CHAR_Value(k) == target_value[1]);
					}
				}
			}
		}
	}
	//至此Res和Sym中分别按顺序存放bool值和逻辑运算符AND OR
	//由于AND优先级大于OR 先计算所有AND
	for (int i = 0; i < Sym.size(); i++)
	{
		if (Sym[i] == "AND")
		{
			Res[i] = Res[i] && Res[i + 1];  //Res应该是比Sym长一位的，所以大概是安全的吧，再看看
			Res.erase(Res.begin() + i + 1); //两项合为一项
			Sym.erase(Sym.begin() + i);		//删掉AND
			i--;							//因为做过删除操作了，i要-1，再看看
		}
	}
	//至此Sym中应该存放的都是OR
	//只需遍历Res，只要有一个正确即返回true，否则返回false
	for (int i = 0; i < Res.size(); i++)
		if (Res[i] == true)
			return true;
	return false;
}

int TABLE::Count(string expression)
{
	this->UpdateRow();
	int count = this->GetRowNum();
	if (expression == "*")
		return count;
	else
	{
		COLUMN *p = this->GetColumn(expression);
		for (int i = 0; i < this->GetRowNum(); i++)
		{
			if (p->Get_IsNull(i))
				count--;
		}
	}
	return count;
}

void TABLE::classify(const vector<string> &group_col)
{
	classifier.clear();
	num_of_each_kind.clear();
	kind = 0;
	classifier.push_back(kind);
	num_of_each_kind.push_back(1);
	this->UpdateRow();
	for (int i = 1; i < this->GetRowNum(); i++) //每次判断第i行的数据是否可以与之前的行归为一组
	{
		int j = 0;
		for (; j < i; j++) //遍历第i行之前的每一行
		{
			bool same = true;
			for (int k = 0; k < group_col.size(); k++) //遍历需要比较的每一列
			{
				COLUMN *pcol = this->GetColumn(group_col[k]);
				switch (this->GetType(group_col[k]))
				{
				case _INT:
					if (*(pcol->Get_INT_Data(i)) != *(pcol->Get_INT_Data(j)))
						same = false;
					break;
				case _DOUBLE:
					if (*(pcol->Get_DOUBLE_Data(i)) != *(pcol->Get_DOUBLE_Data(j)))
						same = false;
					break;
				case _CHAR:
					if (*(pcol->Get_CHAR_Data(i)) != *(pcol->Get_CHAR_Data(j)))
						same = false;
					break;
				}
				if (same == false)
					break;
			}
			if (same)
			{
				classifier.push_back(classifier[j]);
				num_of_each_kind[classifier[j]]++;
				break;
			}
		}
		if (j == i)
		{
			kind++;
			classifier.push_back(kind);
			num_of_each_kind.push_back(1);
		}
	}
}

void TABLE::Select_Group(const std::vector<std::string> &col_name)
{
	/*for (int i = 0; i < classifier.size(); i++)
		cout << classifier[i] << ' ';
	cout << endl;
	for (int i = 0; i <= kind; i++)
		cout << num_of_each_kind[i] << ' ';
	cout << endl;*/

	for (int i = 0; i <= kind; i++) //以种为一行输出
	{
		for (int j = 0; j < this->GetRowNum(); j++) //找到属于那一种的某一行
		{
			bool flag = false;
			if (classifier[j] == i)
			{
				flag = true;
				for (int p = 0; p < col_name.size(); p++) //遍历需要输出的列名
				{
					if (col_name[p].substr(0, 6) == "COUNT(" && col_name[p][col_name[p].length() - 1] == ')')
					{
						string expression = col_name[p].substr(6, col_name[p].length() - 7);
						int count = num_of_each_kind[i];
						if (expression != "*")
						{
							COLUMN *p = this->GetColumn(expression);
							for (int k = 0; k < this->GetRowNum(); k++)
							{
								if (p->Get_IsNull(k) && classifier[k] == i)
									count--;
							}
						}
						cout << count << "\t";
					}
					else
					{
						auto pc = TableMap[col_name[p]];
						auto type = GetType(col_name[p]);
						if (type == _INT)
						{
							if (pc->Get_IsNull(j))
								cout << "NULL\t";
							else
								cout << pc->Get_INT_Value(j) << "\t";
						}
						else if (type == _CHAR)
						{
							if (pc->Get_IsNull(j))
								cout << "NULL\t";
							else
								cout << pc->Get_CHAR_Value(j) << "\t";
						}
						else if (type == _DOUBLE)
						{
							if (pc->Get_IsNull(j))
								cout << "NULL\t";
							else
								cout << pc->Get_DOUBLE_Value(j) << "\t";
						}
					}
				}
				if (flag)
					break;
			}
		}
		cout << endl;
	}
}

void TABLE::write_into_outfile(const std::string &out_file_name, const std::vector<int> &outorder, const std::vector<std::string> &col_name)
{
	ofstream fout;
	fout.open(out_file_name);
	this->UpdateRow();
	if (RowNum == 0)
		return;
	if (outorder.size() == 0)
		return; //如果不用输出，连表头都不需要打
	if (col_name[0] == "*")
	{
		/*
		for (int i = 0; i < ColumnName.size(); i++)
		{
			std::cout << ColumnName[i] << "\t";
		}
		std::cout << std::endl;
		*/
		//文件输出不需要打表头
		for (int j = 0; j < outorder.size(); j++)
		{ //遍历outorder
			for (int i = 0; i < ColumnName.size(); i++)
			{ //遍历所有列
				if (ColumnType[i] == _INT)
				{ //判断列的类型
					auto tem = TableMap[ColumnName[i]];
					if (tem->Get_IsNull(outorder[j]))
						fout << "NULL\t";
					else
						fout << tem->Get_INT_Value(outorder[j]) << "\t";
				}
				else if (ColumnType[i] == _CHAR)
				{
					auto tem = TableMap[ColumnName[i]];
					if (tem->Get_IsNull(outorder[j]))
						fout << "NULL\t";
					else
						fout << tem->Get_CHAR_Value(outorder[j]) << "\t";
				}
				else if (ColumnType[i] == _DOUBLE)
				{
					auto tem = TableMap[ColumnName[i]];
					if (tem->Get_IsNull(outorder[j]))
						fout << "NULL\t";
					else
						fout << fixed << setprecision(4) << tem->Get_DOUBLE_Value(outorder[j]) << "\t";
				}
			}
			fout << '\n';
		}
	}
	//在这里讨论如果“列名”是COUNT(expression)的情况,已知不会有WHERE语句
	/*
	else if (col_name[0].substr(0, 6) == "COUNT(" && col_name[0][col_name[0].length() - 1] == ')')
	{
		cout << col_name[0] << endl;
		string expression = col_name[0].substr(6, col_name[0].length() - 7);
		cout << this->Count(expression) << "\t";
	}
	*/
	//同样的这一部分也不需要了
	else
	{
		if (outorder.size() > 0)
		{ //输出
			/* 
			for (int p = 0; p < col_name.size(); p++)
			{
				cout << col_name[p] << "\t";
			}
			cout << endl;
			*/
			//不需要输出列名了

			for (int j = 0; j < outorder.size(); j++)
			{
				for (int p = 0; p < col_name.size(); p++)
				{

					auto pc = TableMap[col_name[p]];
					auto type = GetType(col_name[p]);
					if (type == _INT)
					{
						if (pc->Get_IsNull(outorder[j]))
							fout << "NULL\t";
						else
							fout << pc->Get_INT_Value(outorder[j]) << "\t";
					}
					else if (type == _CHAR)
					{
						if (pc->Get_IsNull(outorder[j]))
							fout << "NULL\t";
						else
							fout << pc->Get_CHAR_Value(outorder[j]) << "\t";
					}
					else if (type == _DOUBLE)
					{
						if (pc->Get_IsNull(outorder[j]))
							fout << "NULL\t";
						else
							fout << pc->Get_DOUBLE_Value(outorder[j]) << "\t";
					}
				}
				fout << '\n';
			}
		}
	}
	fout.close();
}

void TABLE::load_data_from_file(const std::string &in_file_name, const std::vector<std::string> &col_name)
{
	ifstream fin;
	fin.open(in_file_name, ios::binary);
	if (!fin)
	{
		cout << "ERROR! No such file or directory." << endl;
		return;
	}
	int col_number = col_name.size();
	if (col_number == 0)
	{
		cout << "ERROR! No elected columns." << endl;
		return;
	}
	if (fin.peek() == EOF)
	{
		cout << "ERROR! File is empty." << endl;
		return;
	}
	char ch;
	string content0, line0;

	while (!fin.eof())
	{
		getline(fin, line0);
		istringstream line(line0);
		for (int i = 0; i < col_name.size(); i++)
		{
			content0 = "";
			line >> content0;
			istringstream content(content0);
			if (GetType(col_name[i]) == _INT)
			{
				if (content0 == "NULL")
				{
					TableMap[col_name[i]]->push_back_null(0);
				}
				else
				{
					int k;
					content >> k;
					TableMap[col_name[i]]->push_back(k);
				}
			}
			else if (GetType(col_name[i]) == _DOUBLE)
			{
				if (content0 == "NULL")
				{
					TableMap[col_name[i]]->push_back_null(0.0);
				}
				else
				{
					double k;
					content >> k;
					TableMap[col_name[i]]->push_back(k);
				}
			}
			else if (GetType(col_name[i]) == _CHAR)
			{
				if (content0 == "NULL")
				{
					TableMap[col_name[i]]->push_back_null('\0');
				}
				else
				{
					char k;
					content >> k;
					TableMap[col_name[i]]->push_back(k);
				}
			}
		}
		if(fin.peek() == EOF)break;
	}
	fin.close();
	return;
}
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
		while (temp[0] == ' ')
		{
			temp = temp.substr(1);
		}
		if (temp == "")
			continue;
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

void TABLE::Update(string aname, string avalue, string condition)
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
		cout << "Can not find " << aname << endl;
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
std::string TABLE::Get_KeyColumn()
{
	return KeyColumn;
}

const std::vector<int> &TABLE::Select(const std::vector<std::string> &col_name, string condition)
{ //主要接口,其实这里col_name没有用到
	string copy_condition = condition;
	while (condition[0] == ' ')
	{
		condition = condition.substr(1);
	}
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
	this->bubble_sort(outorder);
	return outorder;
}

void TABLE::bubble_sort(std::vector<int> &order)
{
	if (order.size() > 1)
	{ //冒泡排序
		for (int i = 0; i < (order.size() - 1); i++)
		{
			if (i >= (order.size() - 1))
				break;
			for (int j = 0; j < (order.size() - 1 - i); j++)
			{
				if (this->MyCompare(order[j + 1], order[j], this->Get_KeyColumn()))
				{
					int tmp = order[j + 1];
					order[j + 1] = order[j];
					order[j] = tmp;
				}
			}
		}
	}
}

void TABLE::show_output_from_col(const std::string &colname, const std::vector<int> &outorder, int k)
{
	if (TableMap[colname]->Get_IsNull(k))
	{
		cout << "NULL"
			<< "\t";
	} //为空
	if (GetType(colname) == _INT)
	{
		cout << TableMap[colname]->Get_INT_Value(outorder[k]) << "\t";
	}
	else if (GetType(colname) == _DOUBLE)
	{
		cout << fixed << setprecision(4) << TableMap[colname]->Get_DOUBLE_Value(outorder[k]) << "\t";
	}
	else if (GetType(colname) == _CHAR)
	{
		cout << TableMap[colname]->Get_CHAR_Value(outorder[k]) << "\t";
	} //不为空
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
		cout << this->Count(expression) << endl;
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

void TABLE::Insert(std::string cname, std::string d)
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
		string t = " " ;
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
	string condition_upper;
	transform(condition.begin(), condition.end(), back_inserter(condition_upper), ::toupper);
/*
	'%a'     //以a结尾的数据
	'a%'     //以a开头的数据
	'%a%'    //含有a的数据
	'_a_'    //三位且中间字母是a的
	'_a'     //两位且结尾字母是a的
	'a_'     //两位且开头字母是a的
*/
	if (condition_upper.find(" LIKE ")!=-1)
	{
		stringstream ilike(condition);
		string column_name;
		ilike >> column_name;
		//cout << "column_name: " << column_name << endl;
		string like_expression;
		ilike>> like_expression;
		ilike >> like_expression;//把LIKE关键词给跳过了
		//cout << "like_expression: " << like_expression << endl;
		like_expression = like_expression.substr(1, like_expression.length() - 2);
		//cout << "like_expression: " << like_expression << endl;
		COLUMN *p = this->GetColumn(column_name);
		string key_string;//储存模糊匹配的关键词
		string str = p->Get_CHAR_Data(k)->GetValue();
		if (like_expression[like_expression.length() - 1] == '%'&&like_expression[0] == '%')
		{
			key_string = like_expression.substr(1, like_expression.length() - 2);
			if (str.find(key_string) > 0 && str.find(key_string) < str.length() - key_string.length())
			{
				return 1;
			}
			return 0;
		}
		else if (like_expression[0] == '%')
		{
			key_string = like_expression.substr(1);
			if (key_string.length() > str.length())
			{
				return 0;
			}
			else if (str.substr(str.length() - key_string.length(), key_string.length()) ==key_string)
			{
				return 1;
			}
			else
				return 0;
			
		}
		else if (like_expression[like_expression.length() - 1] == '%')
		{
			key_string = like_expression.substr(0, like_expression.length() - 1);
			if (str.substr(0, key_string.length()) == key_string)
			{
				return 1;
			}
			return 0;
		}
		
		else if (like_expression.length()==3 && like_expression[0] == '_'&&like_expression[2] == '_')
		{
			key_string = like_expression.substr(1, like_expression.length()-1);
			if (str.length() == 3 && str.substr(1,1)== key_string)
			{
				return 1;
			}
			return 0;
		}
		else if (like_expression.length() == 2 && like_expression[0] == '_')
		{
			key_string = like_expression.substr(1);
			if (str.length() == 2 && str.substr(1)==key_string)
			{
				return 1;
			}
			return 0;
		}
		else if (like_expression.length() == 2 && like_expression[1] == '_')
		{
			key_string = like_expression.substr(0,1);
			if (str.length()==2 && str.substr(0,1) == key_string)
			{
				return 1;
			}
			return 0;
		}
		else
		{
			cout << "The LIKE sentence is invalid.Please enter again~" << endl;
			return 0;
		}
	}
	else
	{
		//现分割逐句判断在运算逻辑运算式
		vector<string> Con; //存储> < = 语句，还有"NOT"
		vector<string> Sym; //存储逻辑运算符
		vector<bool> Res;   //存储每一句的bool值
		split(condition, Con);
		bool has_not = false;
		for (int i = 0; i < Con.size(); i++)
		{
			string ini = Con[i], upp;
			upp = "";
			transform(ini.begin(), ini.end(), back_inserter(upp), ::toupper);
			if (upp == "NOT")
			{
				has_not = true;
				Con.erase(Con.begin() + i);
				i--;
				continue;
			}
			//遍历Con，若是< > = 语句则判断bool值，否则为逻辑运算 符，加入到Sym并删除
			if (upp == "AND" || upp == "OR" || upp == "XOR")
			{
				if (upp == "AND")
				{
					Sym.push_back("AND");
				}
				if (upp == "OR")
				{
					Sym.push_back("OR");
				}
				if (upp == "XOR")
				{
					Sym.push_back("XOR");
				}
				Con.erase(Con.begin() + i);
				i--;
				continue;
			}
			//逐个遍历每一句中字符，若找到 > < = 则分割为两段，左为列名，将右边转化为对应列数据类型进行比较得到bool值
			//有一个问题，为什么左边一定是列名，还有一个问题，不支持列与列之间比较吗？？
			//改！！
			//NULL怎么办？？
			//默认语句合法，即：不是列名的内容就一定是常数/字符串
			for (int p = 0; p < Con[i].length(); p++)
			{
				//如果读到NOT证明下一个读到的玩意儿要取not
				if (!(Con[i][p] == '>' || Con[i][p] == '<' || Con[i][p] == '='))
					continue;
				else
				{
					string Symbol = Con[i].substr(p, 1);
					string left_side = Con[i].substr(0, p);
					string right_side = Con[i].substr(p + 1, Con[i].size());
					if (left_side[0] == '\"' || left_side[0] == '\'')
						left_side = left_side.substr(1);
					if (left_side[left_side.length() - 1] == '\"' || left_side[left_side.length() - 1] == '\'')
						left_side = left_side.substr(0, left_side.length() - 1);
					if (right_side[0] == '\"' || right_side[0] == '\'')
						right_side = right_side.substr(1);
					if (right_side[right_side.length() - 1] == '\"' || right_side[right_side.length() - 1] == '\'')
						right_side = right_side.substr(0, right_side.length() - 1);
					int left_index = -1, right_index = -1; //如果这两个东西之后某一个还是-1，代表对应的那一端是常数
					int left_i, right_i;
					double left_d, right_d;
					std::string left_c, right_c; //这六个用来进行常数之间的比较
					DataType left_type, right_type;
					for (int j = 0; j < ColumnName.size(); j++)
					{
						if (left_side == ColumnName[j])
						{
							left_index = j;
							break;
						}
					}
					for (int j = 0; j < ColumnName.size(); j++)
					{
						if (right_side == ColumnName[j])
						{
							right_index = j;
							break;
						}
					}
					//接下来，把列名都转化为常数
					if (left_index == -1 && right_index == -1)
					{
						if ((left_side[0] <= 'Z' && left_side[0] >= 'A') || (left_side[0] <= 'z' && left_side[0] >= 'a'))
						{
							left_type = _CHAR;
							left_c = left_side;
							right_c = right_side;
							if (Symbol == "<")
								Res.push_back(left_c < right_c);
							else if (Symbol == ">")
								Res.push_back(left_c > right_c);
							else if (Symbol == "=")
								Res.push_back(left_c == right_c);
						} //CHAR
						else if (left_side.find('.') != -1 || right_side.find('.') != -1)
						{
							left_type = _DOUBLE;
							left_d = stod(left_side);
							right_d = stod(right_side);
							if (Symbol == "<")
								Res.push_back(left_d < right_d);
							else if (Symbol == ">")
								Res.push_back(left_d > right_d);
							else if (Symbol == "=")
								Res.push_back(left_d == right_d);
						} //DOUBLE
						else
						{
							left_type = _INT;
							left_i = stoi(left_side);
							right_i = stoi(right_side);
							if (Symbol == "<")
								Res.push_back(left_i < right_i);
							else if (Symbol == ">")
								Res.push_back(left_i > right_i);
							else if (Symbol == "=")
								Res.push_back(left_i == right_i);
						}
					} //如果两边都是常数，不可能有NULL
					else
					{
						if (left_index != -1)
						{
							left_type = ColumnType[left_index];
							if (left_type == _INT)
							{
								left_i = TableMap[left_side]->Get_INT_Value(k);
							}
							else if (left_type == _DOUBLE)
							{
								left_d = TableMap[left_side]->Get_DOUBLE_Value(k);
							}
							else if (left_type == _CHAR)
							{
								left_c = TableMap[left_side]->Get_CHAR_Value(k);
							}
						}
						if (right_index != -1)
						{
							right_type = ColumnType[right_index];
							if (right_type == _INT)
							{
								right_i = TableMap[right_side]->Get_INT_Value(k);
							}
							else if (right_type == _DOUBLE)
							{
								right_d = TableMap[right_side]->Get_DOUBLE_Value(k);
							}
							else if (right_type == _CHAR)
							{
								right_c = TableMap[right_side]->Get_CHAR_Value(k);
							}
						} //处理不是常数的左/右端式子

						if (left_index == -1)
						{
							left_type = right_type;
							if (left_type == _INT)
							{
								left_i = stoi(left_side);
							}
							else if (left_type == _DOUBLE)
							{
								left_d = stod(left_side);
							}
							else if (left_type == _CHAR)
							{
								left_c = left_side;
							}
						}
						if (right_index == -1)
						{
							right_type = left_type;
							if (right_type == _INT)
							{
								right_i = stoi(right_side);
							}
							else if (right_type == _DOUBLE)
							{
								right_d = stod(right_side);
							}
							else if (right_type == _CHAR)
							{
								right_c = right_side;
							}
						}
						//处理有一边本来就是常量的情况
						if (left_index != -1)
						{
							if (TableMap[ColumnName[left_index]]->Get_IsNull(k))
							{
								Res.push_back(false);
								break;
							}
						}
						if (right_index != -1)
						{
							if (TableMap[ColumnName[right_index]]->Get_IsNull(k))
							{
								Res.push_back(false);
								break;
							}
						} //如果有NULL直接返回FALSE

						if (Symbol == "<")
						{
							if (left_type == _INT)
							{
								Res.push_back(left_i < right_i);
							}
							else if (left_type == _DOUBLE)
							{
								Res.push_back(left_d < right_d);
							}
							else if (left_type == _CHAR)
							{
								Res.push_back(left_c < right_c);
							}
						}

						else if (Symbol == ">")
						{
							if (left_type == _INT)
							{
								Res.push_back(left_i > right_i);
							}
							else if (left_type == _DOUBLE)
							{
								Res.push_back(left_d > right_d);
							}
							else if (left_type == _CHAR)
							{
								Res.push_back(left_c > right_c);
							}
						}
						else if (Symbol == "=")
						{
							if (left_type == _INT)
							{
								Res.push_back(left_i == right_i);
							}
							else if (left_type == _DOUBLE)
							{
								Res.push_back(left_d == right_d);
							}
							else if (left_type == _CHAR)
							{
								Res.push_back(left_c == right_c);
							}
						}
					}
					break;
				}
			}
			if (has_not)
			{
				Res[Res.size() - 1] = !Res[Res.size() - 1];
				has_not = false;
			}
			//至此Res和Sym中分别按顺序存放bool值和逻辑运算符AND OR XOR
	//由于AND优先级大于OR 先计算所有AND

	/* for(int i = 0; i < Res.size(); i++)cout << "Res[" << i << "] = " << Res[i] << endl;
	for(int i = 0; i < Sym.size(); i++)cout << "Sym[" << i << "] = " << Sym[i] << endl;*/

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
			//至此Sym中应该存放的都是OR和XOR，两者并列`
			while (Sym.size() != 0)
			{
				for (int i = 0; i < Sym.size(); i++)
				{
					if (Sym[i] == "OR")
					{
						Res[i] = Res[i] || Res[i + 1];
						Res.erase(Res.begin() + i + 1);
						Sym.erase(Sym.begin() + i);
						i--;
					}
					else if (Sym[i] == "XOR")
					{
						Res[i] = ((Res[i] && !Res[i + 1]) || (!Res[i] && Res[i + 1]));
						Res.erase(Res.begin() + i + 1);
						Sym.erase(Sym.begin() + i);
						i--;
					} //改！
					break;
				}
			}
			return Res[0];
		}
		return 0;
	}
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

void TABLE::Select_Group(const std::vector<std::string> &col_name, bool has_order)
{
	/*for (int i = 0; i < classifier.size(); i++)
		cout << classifier[i] << ' ';
	cout << endl;
	for (int i = 0; i <= kind; i++)
		cout << num_of_each_kind[i] << ' ';
	cout << endl;*/

	column_to_order.clear();
	for (int i = 0; i <= kind; i++) //以种为一行输出
	{
		for (int j = 0; j < this->GetRowNum(); j++) //找到属于那一种的某一行
		{
			bool flag = false;
			if (classifier[j] == i)
			{
				if (has_order)
				{
					column_to_order.push_back(j);
					break;
				}
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
		if (!has_order)
			cout << endl;
	}
}

void TABLE::Select_Order(const std::vector<std::string> &col_name, const std::string &order_col)
{
	/*for (int i = 0; i < column_to_order.size(); i++)
		cout << column_to_order[i] << ' ';
	cout << endl; */
	if (order_col.substr(0, 6) == "COUNT(" && order_col[order_col.length() - 1] == ')')
	{
		string expression = order_col.substr(6, order_col.length() - 7);
		vector<int> count_array;
		for (int i = 0; i <= kind; i++)
		{
			int count = num_of_each_kind[classifier[column_to_order[i]]];
			if (expression != "*")
			{
				COLUMN *p = this->GetColumn(expression);
				for (int k = 0; k < this->GetRowNum(); k++)
				{
					if (p->Get_IsNull(k) && classifier[k] == classifier[column_to_order[i]])
						count--;
				}
			}
			count_array.push_back(count);
		}

		/*for (int i = 0; i < count_array.size(); i++)
			cout << count_array[i] << ' ';
		cout << endl; */

		for (int i = 0; i < column_to_order.size(); i++)
			for (int j = 0; j < column_to_order.size() - i - 1; j++)
			{
				if (count_array[j] > count_array[j + 1])
				{
					int tmp = count_array[j];
					count_array[j] = count_array[j + 1];
					count_array[j + 1] = tmp;
					tmp = column_to_order[j];
					column_to_order[j] = column_to_order[j + 1];
					column_to_order[j + 1] = tmp;
				}
			}
	}
	else
	{
		COLUMN *pcol = this->GetColumn(order_col);
		for (int i = 0; i < column_to_order.size(); i++)
			for (int j = 0; j < column_to_order.size() - i - 1; j++)
			{
				bool chk = false;
				switch (this->GetType(order_col))
				{
				case _INT:
					if (pcol->Get_INT_Value(j) > pcol->Get_INT_Value(j + 1))
						chk = true;
					break;
				case _DOUBLE:
					if (pcol->Get_DOUBLE_Value(j) > pcol->Get_DOUBLE_Value(j + 1))
						chk = true;
					break;
				case _CHAR:
					if (pcol->Get_CHAR_Value(j) > pcol->Get_CHAR_Value(j + 1))
						chk = true;
					break;
				}
				if (chk)
				{
					int tmp = column_to_order[j];
					column_to_order[j] = column_to_order[j + 1];
					column_to_order[j + 1] = tmp;
				}
			}
	}

	/* for (int i = 0; i < column_to_order.size(); i++)
		cout << column_to_order[i] << ' ';
	cout << endl;*/

	for (int j = 0; j <= kind; j++) //遍历每个种类
	{
		for (int i = 0; i < col_name.size(); i++)
		{
			if (col_name[i].substr(0, 6) == "COUNT(" && col_name[i][col_name[0].length() - 1] == ')')
			{
				string expression = col_name[i].substr(6, col_name[i].length() - 7);
				int count = num_of_each_kind[classifier[column_to_order[j]]];
				if (expression != "*")
				{
					COLUMN *p = this->GetColumn(expression);
					for (int k = 0; k < this->GetRowNum(); k++)
					{
						if (p->Get_IsNull(k) && classifier[k] == classifier[column_to_order[j]])
							count--;
					}
				}
				cout << count << "\t";
			}
			else
			{
				auto pc = TableMap[col_name[i]];
				auto type = GetType(col_name[i]);
				if (type == _INT)
					cout << pc->Get_INT_Value(column_to_order[j]) << "\t";
				else if (type == _CHAR)
					cout << pc->Get_CHAR_Value(column_to_order[j]) << "\t";
				else if (type == _DOUBLE)
					cout << pc->Get_DOUBLE_Value(column_to_order[j]) << "\t";
			}
		}
		cout << endl;
	}
}

void TABLE::Order_in_Union(const std::vector<std::string> &col_name, const std::string &order_col, bool has_all)
{
	column_to_order.clear();
	this->UpdateRow();
	for (int i = 0; i < RowNum; i++)
		column_to_order.push_back(i);
	COLUMN *pcol = this->TableMap[order_col];
	DataType order_type = this->GetType(order_col);
	for (int i = 0; i < RowNum - 1; i++)
		for (int j = 0; j <= RowNum - 2 - i; j++)
		{
			bool chk = false;
			switch (order_type)
			{
			case _INT:
				if (pcol->Get_INT_Value(column_to_order[j]) > pcol->Get_INT_Value(column_to_order[j + 1]))
					chk = true;
				break;
			case _DOUBLE:
				if (pcol->Get_DOUBLE_Value(column_to_order[j]) > pcol->Get_DOUBLE_Value(column_to_order[j + 1]))
					chk = true;
				break;
			case _CHAR:
				if (pcol->Get_CHAR_Value(column_to_order[j]) > pcol->Get_CHAR_Value(column_to_order[j + 1]))
					chk = true;
				break;
			}
			if (chk)
			{
				int tmp = column_to_order[j];
				column_to_order[j] = column_to_order[j + 1];
				column_to_order[j + 1] = tmp;
			}
		}
	if (!has_all)
	{
		vector<int> tmp = column_to_order;
		column_to_order.clear();
		column_to_order.push_back(tmp[0]);
		for (int i = 1; i < tmp.size(); i++)
		{
			bool chk = false;
			switch (order_type)
			{
			case _INT:
				if (pcol->Get_INT_Value(tmp[i]) != pcol->Get_INT_Value(tmp[i - 1]))
					chk = true;
				break;
			case _DOUBLE:
				if (pcol->Get_DOUBLE_Value(tmp[i]) != pcol->Get_DOUBLE_Value(tmp[i - 1]))
					chk = true;
				break;
			case _CHAR:
				if (pcol->Get_CHAR_Value(tmp[i]) != pcol->Get_CHAR_Value(tmp[i - 1]))
					chk = true;
				break;
			}
			if (chk)
				column_to_order.push_back(tmp[i]);
		}
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
	std::map<std::string, bool> col_map;
	if (col_name.size() != ColumnName.size())
	{
		for (int i = 0; i < ColumnName.size(); i++)
		{
			col_map[ColumnName[i]] = false;
		}
		for (int i = 0; i < col_name.size(); i++)
		{
			col_map[col_name[i]] = true; //代表这些列会被输入
		}
		for (int i = 0; i < ColumnName.size(); i++)
		{
			if (!col_map[ColumnName[i]]) //如果这个列没有接受输入数据？？
			{
				cout << "Column " << ColumnName[i] << " does not have any input." << endl;
				if (GetType(ColumnName[i]) == _INT)
				{
					cout << "Automatically filling 0(int) as default." << endl;
				}
				else if (GetType(ColumnName[i]) == _DOUBLE)
				{
					cout << "Automatically filling 0.0(double) as default." << endl;
				}
				else if (GetType(ColumnName[i]) == _CHAR)
				{
					cout << "Automatically filling 0(ascii to char) as default." << endl;
				}
			}
		}
	}

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
					std::string k;
					content >> k;
					TableMap[col_name[i]]->push_back(k);
				}
			}
		}
		if (ColumnName.size() != col_name.size()) //如果有些列存在但是没有被输入值
		{
			for (int i = 0; i < ColumnName.size(); i++)
			{
				if (!col_map[ColumnName[i]]) //如果这个列没有接受输入数据？？
				{
					if (GetType(ColumnName[i]) == _INT)
					{
						TableMap[ColumnName[i]]->push_back(0);
					}
					else if (GetType(ColumnName[i]) == _DOUBLE)
					{
						TableMap[ColumnName[i]]->push_back(0.0);
					}
					else if (GetType(ColumnName[i]) == _CHAR)
					{
						TableMap[ColumnName[i]]->push_back(" ");
					}
				}
			}
		}

		if (fin.peek() == EOF)
			break;
	}
	fin.close();
	return;
}

void TABLE::create_table_file(const std::string &dbname)
{
	UpdateRow();
	std::string filename;
	filename = "./data/" + dbname + "." + name + ".txt";
	std::ofstream fout(filename);
	fout << ColumnName.size() << '\n'; //首先存总列数（空表就存0)
	fout << RowNum << '\n';			   //再存行数
	fout << KeyColumn << '\n';		   //再存一下primary key列的名字
	for (int i = 0; i < ColumnName.size(); i++)
	{
		fout << ColumnName[i] << '\n';
		switch (ColumnType[i])
		{
		case _INT:
			fout << "_INT" << '\n';
			break;
		case _DOUBLE:
			fout << "_DOUBLE" << '\n';
			break;
		case _CHAR:
			fout << "_CHAR" << '\n';
			break;
		default:
			break;
		}
	} //先存每一列的名字及类型，都由换行符分隔
	//等下再存一下每一列的数据
	for (int i = 0; i < ColumnName.size(); i++)
	{
		if (GetType(ColumnName[i]) == _INT)
		{
			UpdateRow();
			for (int j = 0; j < RowNum; j++)
			{
				if (TableMap[ColumnName[i]]->Get_IsNull(j))
					fout << "NULL" << '\t';
				else
					fout << TableMap[ColumnName[i]]->Get_INT_Value(j) << '\t';
			}
		}
		else if (GetType(ColumnName[i]) == _DOUBLE)
		{
			UpdateRow();
			for (int j = 0; j < RowNum; j++)
			{
				if (TableMap[ColumnName[i]]->Get_IsNull(j))
					fout << "NULL" << '\t';
				else
					fout << fixed << setprecision(4) << TableMap[ColumnName[i]]->Get_DOUBLE_Value(j) << '\t';
			}
		}
		else if (GetType(ColumnName[i]) == _CHAR)
		{
			UpdateRow();
			for (int j = 0; j < RowNum; j++)
			{
				if (TableMap[ColumnName[i]]->Get_IsNull(j))
					fout << "NULL" << '\t';
				else
					fout << TableMap[ColumnName[i]]->Get_CHAR_Value(j) << '\t';
			}
		}
		fout << '\n';
	}
	fout.close();
}

void TABLE::load_table(const std::string &dbname)
{
	fstream fst;
	string filename;
	filename = "./data/" + dbname + "." + name + ".txt";
	fst.open(filename);
	int col_num, row_num;
	vector<string> col_name;
	vector<DataType> col_type;
	string name, type, key;
	fst >> col_num;
	fst.get();
	fst >> row_num;
	fst.get();
	fst >> key;
	fst.get();
	for (int i = 0; i < col_num; i++)
	{
		fst >> name;
		fst >> type;
		col_name.push_back(name);
		if (type == "_INT")
		{
			addcolumn(name, _INT);
			col_type.push_back(_INT);
		}
		else if (type == "_DOUBLE")
		{
			addcolumn(name, _DOUBLE);
			col_type.push_back(_DOUBLE);
		}
		else if (type == "_CHAR")
		{
			addcolumn(name, _CHAR);
			col_type.push_back(_CHAR);
		}
		fst.get(); //换行符
	}
	SetKey(key);

	for (int i = 0; i < col_num; i++)
	{
		string data;
		if (col_type[i] == _INT)
		{
			for (int j = 0; j < row_num; j++)
			{
				fst >> data;
				if (data == "NULL")
					TableMap[col_name[i]]->push_back_null(int(0));
				else if (data != "")
					TableMap[col_name[i]]->push_back(stoi(data));
			}
		}
		else if (col_type[i] == _DOUBLE)
		{
			for (int j = 0; j < row_num; j++)
			{
				fst >> data;
				if (data == "NULL")
					TableMap[col_name[i]]->push_back_null(0.0);
				else if (data != "")
					TableMap[col_name[i]]->push_back(stod(data));
			}
		}
		else if (col_type[i] == _CHAR)
		{
			for (int j = 0; j < row_num; j++)
			{
				fst >> data;
				if (data == "NULL")
					TableMap[col_name[i]]->push_back_null(" ");
				else if (data != "")
					TableMap[col_name[i]]->push_back(data);
			}
		}
		fst.get();
	}
	fst.close();
}
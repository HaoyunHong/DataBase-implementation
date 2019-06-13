#include "PARSE.h"
using namespace std;

DataType PARSE::attype(std::string type) //获得数据类型
{
	if (type == "INT")
		return _INT;
	else if (type == "CHAR")
		return _CHAR;
	else if (type == "DOUBLE")
		return _DOUBLE;
	return _INT;
}

void PARSE::Transform(string &src, string &dst) //关键字转换为大写
{
	transform(src.begin(), src.end(), back_inserter(dst), ::toupper);
}

void PARSE::EXEC(ALLBASES &Allbases, string input) //输入命令处理
{
	istringstream is(input); //一行作为输入流，利用>>和getline进行处理
	string ele, ele1;		 //ele及ele(k)是切割得到的原始字段及转换后的字段
	getline(is, ele, ' ');
	Transform(ele, ele1);
	if (ele1 == "CREATE") //先按第一个关键字进行分析，下再按第二个关键字进一步分析
	{
		string ele2;
		getline(is, ele, ' ');
		Transform(ele, ele2);
		if (ele2 == "DATABASE")
		{
			string ele3;
			getline(is, ele3, ';');
			Allbases.create(ele3);
		}
		if (ele2 == "TABLE")
		{
			vector<ConstraintType> tem;
			tem.push_back(NOT_NULL);
			string tbname, cur, cur0, curin, clname, ctype, ctype0; //变量涉及表名、当前读取字段、转换后字段、待存储字段、列名、列类型
			getline(is, tbname, '(');
			curTb = new TABLE;
			curDb->addtable(tbname, curTb);
			getline(is, clname, ' ');
			is >> ctype;
			for (int k = 0; ctype[k]; k++)
			{
				if (ctype[k] == ',')
				{
					ctype.erase(k, 1);
					k--;
				}
			}
			Transform(ctype, ctype0);
			is >> cur;
			Transform(cur, cur0);
			if (cur0 == "NOT") // 对是否存在NOT NULL的处理
			{
				curTb->addcolumn(clname, attype(ctype), tem);
				is >> cur;
				is >> cur;
			}
			else
				curTb->addcolumn(clname, attype(ctype));
			cur0.erase();
			Transform(cur, cur0);
			while (cur0 != "PRIMARY") //PRIMARY KEY 前字段处理
			{
				clname = cur;
				is >> cur;
				curin.erase();
				Transform(cur, curin);
				if (curin == "INT,")
				{
					ctype = "INT";
					curTb->addcolumn(clname, attype(ctype));
				}
				else if (curin == "CHAR,")
				{
					ctype = "CHAR";
					curTb->addcolumn(clname, attype(ctype));
				}
				else if (curin == "DOUBLE,")
				{
					ctype = "DOUBLE";
					curTb->addcolumn(clname, attype(ctype));
				}
				else
				{
					ctype = curin;
					is >> cur;
					curin.erase();
					Transform(cur, curin);
					if (curin == "NOT")
					{
						curTb->addcolumn(clname, attype(ctype), tem);
						is >> cur;
					}
				}
				is >> cur;
				cur0.erase();
				Transform(cur, cur0);
			}
			string key, keyname; //PRIMARY KEY字段处理
			getline(is, key, '(');
			getline(is, keyname, ')');
			curTb->SetKey(keyname);
			is >> cur; //PRIMARY KEY后字段判断及处理
			if (cur == ",")
			{
				while (is >> cur)
				{
					clname = cur;
					is >> cur;
					curin.erase();
					Transform(cur, curin);
					if (curin == "INT," || curin == "INT);")
					{
						ctype = "INT";
						curTb->addcolumn(clname, attype(ctype));
					}
					else if (curin == "CHAR," || curin == "CHAR);")
					{
						ctype = "CHAR";
						curTb->addcolumn(clname, attype(ctype));
					}
					else if (curin == "DOUBLE," || curin == "CHAR);")
					{
						ctype = "DOUBLE";
						curTb->addcolumn(clname, attype(ctype));
					}
					else
					{
						ctype = curin;
						is >> cur;
						curin.erase();
						Transform(cur, curin);
						if (curin == "NOT")
						{
							curTb->addcolumn(clname, attype(ctype), tem);
							is >> cur;
						}
					}
				}
			}
		}
	}
	else if (ele1 == "DROP")
	{
		string ele2;
		getline(is, ele, ' ');
		Transform(ele, ele2);
		if (ele2 == "DATABASE")
		{
			string ele3;
			getline(is, ele3, ';');
			Allbases.del(ele3);
		}
		if (ele2 == "TABLE")
		{
			string ele3;
			getline(is, ele3, ';');
			curDb->deltable(ele3);
		}
	}
	else if (ele1 == "USE")
	{
		string ele2;
		getline(is, ele2, ';');
		curdbname = ele2;
		for (auto it = Allbases.AllBasesMap.begin(); it != Allbases.AllBasesMap.end(); it++)
		{
			if (it->first == ele2)
			{
				curDb = it->second;
				break;
			}
		}
	}
	else if (ele1 == "SHOW")
	{
		string ele2;
		getline(is, ele, ' ');
		Transform(ele, ele2);
		if (ele2 == "DATABASES;")
		{
			Allbases.show();
		}
		else if (ele2 == "TABLES;")
		{
			cout << "Tables_in_" << curdbname << endl;
			curDb->showtables();
		}
		else if (ele2 == "COLUMNS")
		{
			string ele3;
			getline(is, ele3, ' ');
			getline(is, ele3, ';');
			for (auto it = curDb->DataBaseMap.begin(); it != curDb->DataBaseMap.end(); it++)
			{
				if (it->first == ele3)
					curTb = it->second;
			}
			curTb->showcolumns();
		}
	}
	else if (ele1 == "INSERT")
	{
		string ele2;
		getline(is, ele2, ' ');
		vector<string> attr;						  //存储输入信息中的列名
		vector<string> aval;						  //存储输入信息中的值
		vector<string> alcl = curTb->GetColumnName(); //获得当前表的全部列名，供检查是否有未输入
		string tbname, clname, clname0, value;		  //表名、列名、列值
		getline(is, tbname, '(');
		for (auto it = curDb->DataBaseMap.begin(); it != curDb->DataBaseMap.end(); it++)
		{
			if (it->first == tbname)
			{
				curTb = it->second;
				break;
			}
		}
		is >> clname;
		Transform(clname, clname0);
		while (clname0 != "VALUES") //列名处理
		{
			for (int j = 0; clname[j]; j++)
			{
				if (clname[j] == ',' || clname[j] == ')')
				{
					clname.erase(j, 1);
					j--;
				}
			}
			attr.push_back(clname);
			is >> clname;
			clname0.erase();
			Transform(clname, clname0);
		}
		while (is >> value) //列值处理
		{
			for (int j = 0; value[j]; j++)
			{
				if (value[j] == ',' || value[j] == ')' || value[j] == ';' || value[j] == '(' || value[j] == '\"')
				{
					value.erase(j, 1);
					j--;
				}
			}
			aval.push_back(value);
		}
		for (auto it = curDb->DataBaseMap.begin(); it != curDb->DataBaseMap.end(); it++)
		{
			if (it->first == tbname)
			{
				curTb = it->second;
				break;
			}
		}
		auto ita = attr.begin(), itv = aval.begin();
		for (; ita != attr.end() && itv != aval.end(); ita++, itv++)
		{
			for (auto it = alcl.begin(); it != alcl.end(); it++) //删去输入列，留下未输入列
			{
				if (*it == *ita)
				{
					it = alcl.erase(it);
					if (it == alcl.end())
						break;
				}
			}
			DataType type = curTb->GetType(*ita);
			if (type == 0)
			{
				if (*itv == "NULL")
				{
					curTb->InsertNull(*ita, _INT);
				}
				else
				{
					int val = stoi(*itv);
					curTb->Insert(*ita, val);
				}
			}
			else if (type == 2)
			{
				if (*itv == "NULL")
				{
					curTb->InsertNull(*ita, _CHAR);
				}
				else
				{
					char val = (*itv)[0];
					curTb->Insert(*ita, val);
				}
			}
			else if (type == 1)
			{
				if (*itv == "NULL")
				{
					curTb->InsertNull(*ita, _DOUBLE);
				}
				else
				{
					double val = stod(*itv);
					curTb->Insert(*ita, val);
				}
			}
		}
		if (!alcl.empty()) //未输入列判断及处理
		{
			for (auto it = alcl.begin(); it != alcl.end(); it++)
			{
				string nullname = *it;
				DataType type = curTb->GetType(nullname);
				curTb->InsertNull(nullname, type);
			}
		}
	}
	else if (ele1 == "DELETE")
	{
		string ele2, condition; //condition存储whereclause语句，下同
		is >> ele2;
		is >> ele2;
		for (auto it = curDb->DataBaseMap.begin(); it != curDb->DataBaseMap.end(); it++)
		{
			if (it->first == ele2)
			{
				curTb = it->second;
				break;
			}
		}
		is >> ele2;
		getline(is, ele2, ' ');
		getline(is, condition, ';');
		curTb->Delete(condition);
	}
	else if (ele1 == "UPDATE")
	{
		string ele2, mclname, mvalue, condition; //待修改列名，修改值
		is >> ele2;
		for (auto it = curDb->DataBaseMap.begin(); it != curDb->DataBaseMap.end(); it++)
		{
			if (it->first == ele2)
			{
				curTb = it->second;
				break;
			}
		}
		getline(is, ele2, ' ');
		getline(is, ele2, ' ');
		getline(is, mclname, '=');
		is >> mvalue;
		for (int i = 0; mvalue[i]; i++)
		{
			if (mvalue[i] == '\"' || mvalue[i] == '\'')
			{
				mvalue.erase(i, 1);
				i--;
			}
		}
		getline(is, ele2, ' ');
		getline(is, condition, ';');
		DataType type = curTb->GetType(mclname);
		if (type == 0)
		{
			int val = stoi(mvalue);
			curTb->Update(mclname, val, condition);
		}
		else if (type == 2)
		{
			char val = mvalue[0];
			curTb->Update(mclname, val, condition);
		}
		else if (type == 1)
		{
			double val = stod(mvalue);
			curTb->Update(mclname, val, condition);
		}
	}

	else if (ele1 == "SELECT")
	//SELECT COLNAME FROM TBNAME WHERE ...;
	//SELECT COLNAME INTO OUTPUTFILE FROM TBNAME WHERE ...;
	//SELECT COUNT(expression) from TBNAME; (这里如果出现COUNT就不会出现其他列名,和第一种情况一起处理)
	//SELECT stu_name, COUNT(expression) from TBNAME GROUP BY stu_name, ...; (这里SELECT,COUNT只能作用于GROUP的列）
	//SELECT stu_name, COUNT(expression) from TBNAME GROUP BY stu_name, ... ORDER BY ...; (这里ORDER只能作用于GROUP的列或者COUNT）
	{
		std::vector<std::string> col_name;
		col_name.clear();
		string ele2, ele2_upper;
		int chk = 0;				 //1代表正常SELECT，2代表需要输出到文件（出现“INTO”）,3代表……
		bool has_whereclause = true; //判断是否有whereclause语句（现在含义扩大，含有GROUP也将算入）
		while (chk == 0)
		{
			string input_upper;
			Transform(input, input_upper);
			if (input_upper.find("+") != -1 || input_upper.find("-") != -1 || input_upper.find("/") != -1 || input_upper.find("%") != -1 || input_upper.find(" DIV ") != -1 || input_upper.find(" MOD ") != -1)
			{
				stringstream iss(input_upper); //读进来作计算器处理
				string select;
				iss >> select;
				string whole_expression = input_upper.substr(6);
				whole_expression = whole_expression_standardize(whole_expression);
				cout << whole_expression << endl;
				string num_result = arithmetic_calculator(whole_expression);
				cout << setprecision(3) << num_result << endl; //保留三位有效数字
				break;
			}
			else
			{
				is >> ele2;
				if (ele2[ele2.length() - 1] == ',')
				{
					ele2 = ele2.substr(0, ele2.length() - 1);
				} //如果是逗号，去掉
				ele2_upper = "";
				Transform(ele2, ele2_upper);
				if (ele2_upper == "FROM")
				{
					chk = 1;
					break;
				}
				if (ele2_upper == "INTO")
				{
					chk = 2;
					break;
				}
				else //正在读入的还是一个列名
				{
					col_name.push_back(ele2);
				}
			}
		}
		//现在有两种情况：第一种：读到了FROM，没有INTO，第二种，读到了INTO，FROM还在后面
		if (chk == 1) //第一种情况，读到了FROM，证明没有INTO
		{
			string tbname;
			is >> tbname;
			if (tbname[tbname.length() - 1] == ';') //表名后直接是分号，代表没有whereclause
			{
				has_whereclause = false;
				tbname = tbname.substr(0, tbname.length() - 1);
			}
			for (auto it = curDb->DataBaseMap.begin(); it != curDb->DataBaseMap.end(); it++)
			{
				if (it->first == tbname)
				{
					curTb = it->second;
					break;
				}
				if (it == curDb->DataBaseMap.end())
					cout << "CAN NOT FIND TABLENAME" << tbname << endl;
			}						   //利用ele3里面的TBNAME找对应表的指针,其实有函数可以用hhh
			string condition = "true"; //condition缺省为“true”，供无whereclause语句时使用
			if (has_whereclause)
			{
				string condition_upper = "";
				is >> condition; //先把后一个要么是where要么是group的串读进condition
				Transform(condition, condition_upper);
				if (condition_upper == "WHERE") //如果是where，证明是一个输出的语句，按正常情况操作
				{
					condition = "";
					getline(is, condition, ';');						//把whereclause读进condition进行后续操作
					auto outorder = curTb->Select(col_name, condition); //处理，找到符合条件的行存入outorder静态vector
					curTb->show_output_from_select(col_name, outorder); //把符合条件的每一行输出
				}
				else if (condition_upper == "GROUP") //如果读到了group
				{
					is >> condition;		  //读入by,接下来是GROUP要作用的列名
					vector<string> group_col; //储存GROUP作用的列名
					bool has_order = true;	//是否含有ORDER语句
					string _group;
					is >> _group;
					while (_group[_group.length() - 1] == ',')
					{
						group_col.push_back(_group.substr(0, _group.length() - 1));
						is >> _group;
					}
					if (_group[_group.length() - 1] == ';') //如果直接分号结束，说明没有ORDER语句
					{
						has_order = false;
						group_col.push_back(_group.substr(0, _group.length() - 1));
					}
					else
						group_col.push_back(_group);
					curTb->classify(group_col);

					if (has_order) //如果有ORDER
					{
						for (int i = 0; i < col_name.size(); i++)
							cout << col_name[i] << "\t";
						cout << endl;
						curTb->Select_Group(col_name, has_order);
						string order_col;
						is >> order_col; //读入ORDER
						is >> order_col; //读入BY
						is >> order_col;
						order_col = order_col.substr(0, order_col.size() - 1);
						curTb->Select_Order(col_name, order_col);
					}
					else
					{
						for (int i = 0; i < col_name.size(); i++)
							cout << col_name[i] << "\t";
						cout << endl;
						curTb->Select_Group(col_name, has_order);
					}
				}
			}
			else //没有whereclause，全部输出
			{
				auto outorder = curTb->Select(col_name, "true");
				curTb->show_output_from_select(col_name, outorder);
			}
		}
		else if (chk == 2) //SELECT * INTO OUTFILE 'output_file' FROM oop_info WHERE whereclause;
		{
			string ele0, out_file_name, tbname;
			string condition = "true";   //模仿上面的操作，缺省值供没有whereclause时使用
			bool has_whereclause = true; //指示是否存在whereclause
			is >> ele0;					 //输入"OUTFILE"
			is >> out_file_name;		 //输出文件名
			if (out_file_name[0] == '\'' || out_file_name[0] == '\"')
				out_file_name = out_file_name.substr(1, out_file_name.length() - 1);
			if (out_file_name[out_file_name.length() - 1] == '\'' || out_file_name[out_file_name.length() - 1] == '\"')
				out_file_name = out_file_name.substr(0, out_file_name.length() - 1); //去掉引号
			is >> ele0;																 //输入"FROM"
			is >> tbname;															 //表名
			if (tbname[tbname.length() - 1] == ';')
			{
				tbname = tbname.substr(0, tbname.length() - 1);
				has_whereclause = false;
			}								 //没有whereclause，输出全部行
			curTb = curDb->GetTable(tbname); //切换当前表
			if (has_whereclause)
			{
				is >> ele0; //输入"WHERE"
				condition = "";
				getline(is, condition, ';'); //读入condition
			}
			auto outorder = curTb->Select(col_name, condition); //outorder储存了需要输出的行的下标
			curTb->write_into_outfile(out_file_name, outorder, col_name);
		}
	}

	else if (ele1 == "LOAD") //LOAD DATA INFILE 'dump.txt' INTO TABLE mytbl(b, c, a);
	{
		string ele0, ele0_upper, file_name, tbname, colname;
		is >> ele0; //"DATA"
		ele0_upper = "";
		Transform(ele0, ele0_upper);
		if (ele0_upper != "DATA")
		{
			cout << ele0_upper << endl;
			cout << "invalid command :: expect \"DATA\" here but not found.";
			return;
		}
		is >> ele0; //"INFILE"
		ele0_upper = "";
		Transform(ele0, ele0_upper);
		if (ele0_upper == "LOCAL")
		{
			is >> ele0;
			ele0_upper = "";
			Transform(ele0, ele0_upper);
		}
		if (ele0_upper != "INFILE")
		{
			cout << "invalid command :: expect \"INFILE\" here but not found." << endl;
			return;
		}
		is >> file_name;
		if (file_name[0] == '\'' || file_name[0] == '\"')
		{
			file_name = file_name.substr(1, file_name.length() - 1);
		}
		if (file_name[file_name.length() - 1] == '\'' || file_name[file_name.length() - 1] == '\"')
		{
			file_name = file_name.substr(0, file_name.length() - 1);
		}			//去掉引号
		is >> ele0; //"INTO"
		ele0_upper = "";
		Transform(ele0, ele0_upper);
		if (ele0_upper != "INTO")
		{
			cout << "invalid command :: expect \"INTO\" here but not found.";
			return;
		}
		is >> ele0; //"TABLE"
		ele0_upper = "";
		Transform(ele0, ele0_upper);
		if (ele0_upper != "TABLE")
		{
			cout << "invalid command :: expect \"TABLE\" here but not found.";
			return;
		}
		getline(is, tbname, ';');
		while (tbname[0] == ' ')
		{
			tbname = tbname.substr(1, tbname.length() - 1);
		} //要记得把最前面的空格去掉
		std::vector<std::string> col_name;
		col_name.clear();
		bool chk = true; //指示是否没有对输入列作要求
		if (tbname[tbname.length() - 1] == ')')
		{
			int pos = tbname.find('(');
			string col;
			string col_list = tbname.substr(pos + 1, tbname.length() - pos - 1); //去掉了括号，只含由逗号分隔开的列名
			while (col_list.find(',') != -1)
			{
				pos = col_list.find(',');
				col = col_list.substr(0, pos);
				col_list = col_list.substr(pos + 2);
				col_name.push_back(col);
			}
			if (col_list[col_list.length() - 1] == ')')
			{
				col_list = col_list.substr(0, col_list.length() - 1);
			}
			if (col_list.length() != 0)
				col_name.push_back(col_list);
			tbname = tbname.substr(0, tbname.find('('));
			chk = false;
		} //有输入顺序的要求

		curTb = curDb->GetTable(tbname);
		curTb->UpdateRow();
		if (chk) //如果对输入列没有要求，只有表名，就把每一列的名字都放进col_name里面去
		{
			for (int i = 0; i < curTb->GetColumnName().size(); i++)
			{
				col_name.push_back(curTb->GetColumnName()[i]);
			}
		}
		curTb->load_data_from_file(file_name, col_name);
	}
}

string PARSE::whole_expression_standardize(string whole_expression)
{
	string copy_whole_expression = whole_expression + ";";
	int length = whole_expression.length();
	int i = 1;
	while (i < copy_whole_expression.length() && copy_whole_expression[i] != ':')
	{
		if (copy_whole_expression[i] == '+' || copy_whole_expression[i] == '-' || copy_whole_expression[i] == '*' || copy_whole_expression[i] == '/' || copy_whole_expression[i] == '%')
		{
			copy_whole_expression = copy_whole_expression.substr(0, i) + " " + copy_whole_expression.substr(i, 1) + " " + copy_whole_expression.substr(i + 1);
			i += 2;
		}
		else
		{
			i++;
		}
	}
	return copy_whole_expression.substr(0, i - 1);
}

int PARSE::P(string arithmetic_operator) //制定运算符优先级
{
	if (arithmetic_operator == "+" || arithmetic_operator == "-")
	{
		return 1;
	}
	if (arithmetic_operator == "*" || arithmetic_operator == "/" || arithmetic_operator == "DIV" || arithmetic_operator == "%" || arithmetic_operator == "MOD")
	{
		return 2;
	}
	if (arithmetic_operator == "#")
	{
		return 0;
	}
	return -1;
}

//算数计算器
string PARSE::arithmetic_calculator(string &whole_expression)
{
	istringstream s(whole_expression + " # "); //"#"表示算术表达式结束
	std::stack<string> arithmetic_operators;   //运算符栈
	std::stack<double> numbers;				   //数据栈
	arithmetic_operators.push("#");
	string arithmetic;
	bool used = true;
	while (!arithmetic_operators.empty())
	{
		if (s && used)
		{
			s >> arithmetic;
		}
		//cout << "arithmetic: " << arithmetic << endl;
		if (arithmetic != "+" && arithmetic != "-" && arithmetic != "*" && arithmetic != "/" && arithmetic != "DIV" && arithmetic != "%" && arithmetic != "MOD" && arithmetic != "#")
		{
			numbers.push(stod(arithmetic));
			//cout << "temp_numbers.top(): " << numbers.top() << endl;
			continue;
		}

		else
		{
			if (P(arithmetic_operators.top()) < P(arithmetic))
			{
				used = true;
				/*cout << "P(arithmetic_operators.top()): " << P(arithmetic_operators.top()) << endl;
				cout << "P(arithmetic): " << P(arithmetic) << endl;
				cout << "temp_arithmetic_operators.top(): "<<arithmetic_operators.top() << endl;
				cout << "temp_arithmetic: "<< arithmetic << endl;*/
				arithmetic_operators.push(arithmetic);
			}
			else
			{
				if (arithmetic == "#" && arithmetic_operators.top() == "#")
				{
					break;
				}
				used = false;
				double number2 = numbers.top();
				numbers.pop();
				double number1 = numbers.top();
				numbers.pop();
				string op = arithmetic_operators.top();
				arithmetic_operators.pop();
				if (op == "+")
				{
					numbers.push(number1 + number2);
				}
				if (op == "-")
				{
					numbers.push(number1 - number2);
				}
				if (op == "*")
				{
					numbers.push(number1 * number2);
				}
				if (op == "/" || op == "DIV")
				{
					string number2_upper = to_string(number2);
					//？？？不知为何，这里编译器识别不出来
					//Transform(to_string(number2),number2_upper);
					if (number2_upper == "NULL")
					{
						return "NULL";
					}
					numbers.push(number1 / number2);
				}
				if (op == "%" || op == "MOD")
				{
					string number2_upper = to_string(number2);
					//Transform(to_string(number2),number2_upper);
					if (number2_upper == "NULL")
					{
						return "NULL";
					}
					numbers.push(int(number1) % int(number2));
				}
			}
		}
		/*if (arithmetic == "#" && arithmetic_operators.top() == "#")
		{
			cout << "arithmetic_operators.top(): " << arithmetic_operators.top() << endl;
			//arithmetic_operators.pop();
			break;
		}
		cout << "arithmetic_operators.top(): " << arithmetic_operators.top() << endl;
		cout << "numbers.top(): " << numbers.top() << endl;*/
	}
	return to_string(numbers.top());
}
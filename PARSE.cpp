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
			Allbases.create_allbases_file();
		}
		if (ele2 == "TABLE")
		{
			vector<ConstraintType> tem;
			tem.push_back(NOT_NULL);
			string tbname, cur, cur0, curin, clname, ctype, ctype0; //变量涉及表名、当前读取字段、转换后字段、待存储字段、列名、列类型
			getline(is, tbname, '(');
			curTb = new TABLE(tbname);
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
			curDb->create_database_file();
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
			Allbases.create_allbases_file();
		}
		if (ele2 == "TABLE")
		{
			string ele3;
			getline(is, ele3, ';');
			curDb->deltable(ele3);
			curDb->create_database_file();
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
					//std::string val = (*itv)[0];
					std::string val = (*itv);
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
		curTb->create_table_file(curDb->get_name());
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
		curTb->create_table_file(curDb->get_name());
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
		curTb->create_table_file(curDb->get_name());
	}

	else if (ele1 == "SELECT")
		//SELECT COLNAME FROM TBNAME WHERE ...;
		//SELECT COLNAME INTO OUTPUTFILE FROM TBNAME WHERE ...;
		//SELECT COUNT(expression) from TBNAME; (这里如果出现COUNT就不会出现其他列名,和第一种情况一起处理)
		//SELECT stu_name, COUNT(expression) from TBNAME GROUP BY stu_name, ...; (这里SELECT,COUNT只能作用于GROUP的列）
		//SELECT stu_name, COUNT(expression) from TBNAME GROUP BY stu_name, ... ORDER BY ...; (这里ORDER只能作用于GROUP的列或者COUNT）
		//SELECT stu_name, ... FROM TBNAME1 UNION (ALL) SELECT stu_name, ... FROM TBNAME2 ORDER BY stu_name; (这里默认第一个出现的列为整合列，ORDER只能作用于整合列,...中的列数目必须相同,表头按TBNAME1的列名输出)
	{
		std::vector<std::string> col_name;
		std::vector<std::string> tb_name;
		std::vector<std::string> col_tb_name;
		std::vector<std::string> col_2_name;
		col_name.clear();
		string ele2, ele2_upper;
		int chk = 0;				 //1代表正常SELECT,2代表需要输出到文件（出现“INTO”）,3代表UNION命令
		bool has_whereclause = true; //判断是否有whereclause语句（现在含义扩大，含有GROUP也将算入）
		while (chk == 0)
		{
			string input_upper;
			Transform(input, input_upper);
			/**此处是select后面关于字符串的小函数**/
			if (input_upper.find(" AS ") != -1 || input_upper.find(")AS ") != -1)
			{
				//此处实现的两个计算字符串长度的函数（同功能）
				if (input_upper.find(" CHAR_LENGTH(") != -1 || input_upper.find(" CHARACTER_LENGTH(") != -1)
				{
					string str = input.substr(input.find("(") + 2, input.find(")") - input.find("(") - 3);
					cout << "The length of the entered string \"" << str << "\" is " << str.length() << "." << endl;
				}
				//此处是实现连接字符串的函数
				else if (input_upper.find(" CONCAT(") != -1)
				{
					string str = input.substr(input.find("(") + 1, input.find(")") - input.find("(") - 1);
					//cout << str << endl;
					string whole_str;
					while (str.find(", ") != -1)
					{
						string add_str = str.substr(str.find("\"") + 1, str.find(", ") - str.find("\"") - 2);
						//cout << "add_str: "<<add_str << endl;
						whole_str += add_str;
						str = str.substr(str.find(", ") + 2);
					}
					whole_str += str.substr(str.find("\"") + 1, str.length() - 2);
					cout << "The merged string is \"" << whole_str << "\"." << endl;
				}
				//合并多个字符串并添加分隔符
				else if (input_upper.find(" CONCAT_WS(") != -1)
				{
					string str = input.substr(input.find("(") + 1, input.find(")") - input.find("(") - 1);
					//cout << str << endl;
					string whole_str;
					string seperation;
					bool is_seperation = true;
					while (str.find(", ") != -1)
					{
						string add_str = str.substr(str.find("\"") + 1, str.find(", ") - str.find("\"") - 2);
						if (is_seperation)
						{
							seperation = add_str;
							is_seperation = false;
						}
						else
						{
							//cout << "add_str: "<<add_str << endl;
							whole_str += add_str + seperation;

						}
						str = str.substr(str.find(", ") + 2);
					}
					whole_str += str.substr(str.find("\"") + 1, str.length() - 2);
					cout << "The merged string with entered seperation is \"" << whole_str << "\"." << endl;
				}
				break;
			}
			/**此处是数字函数**/
			if (input_upper.find(" ABS(") != -1)
			{
				string str = input.substr(input.find("(") + 1, input.find(")") - input.find("(") - 1);
				//cout << str << endl;
				cout << abs(stod(str)) << endl;
				break;
			}
			if (input_upper.find(" ACOS(") != -1)
			{
				string str = input.substr(input.find("(") + 1, input.find(")") - input.find("(") - 1);
				//cout << str << endl;
				cout << acos(stod(str)) * 180 / 3.1415926535 << " degree" << endl;
				break;
			}
			if (input_upper.find(" ASIN(") != -1)
			{
				string str = input.substr(input.find("(") + 1, input.find(")") - input.find("(") - 1);
				//cout << str << endl;
				cout << asin(stod(str)) * 180 / 3.1415926535 << " degree" << endl;
				break;
			}
			if (input_upper.find(" ATAN(") != -1)
			{
				string str = input.substr(input.find("(") + 1, input.find(")") - input.find("(") - 1);
				//cout << str << endl;
				cout << atan(stod(str)) * 180 / 3.1415926535 << " degree" << endl;
				break;
			}
			if (input_upper.find(" CEIL(") != -1 || input_upper.find(" CEILING(") != -1)
			{
				string str = input.substr(input.find("(") + 1, input.find(")") - input.find("(") - 1);
				//cout << str << endl;
				cout << stoi(str) + 1 << endl;
				break;
			}

			/**再加两个判断，防止影响后面**/
			else if (input_upper.find(" FROM ") == -1 && input_upper.find(" INTO ") == -1 && (input_upper.find("+") != -1 || input_upper.find("-") != -1 || input_upper.find("*") != -1 || input_upper.find("/") != -1 || input_upper.find("%") != -1 || input_upper.find(" DIV ") != -1 || input_upper.find(" MOD ") != -1|| input_upper.find(" AND ") != -1 || input_upper.find(" OR ") != -1 || input_upper.find(" XOR ") != -1 || input_upper.find(" NOT ") != -1 || input_upper.find(" ! ") != -1))
			{
				stringstream iss(input_upper); //读进来作计算器处理
				string select;
				iss >> select;
				string whole_expression = input_upper.substr(7);
				whole_expression = whole_expression_standardize(whole_expression);
				if (input_upper.find("+") != -1 || input_upper.find("-") != -1 || input_upper.find("*") != -1 || input_upper.find("/") != -1 || input_upper.find("%") != -1 || input_upper.find(" DIV ") != -1 || input_upper.find(" MOD ") != -1)
				{
					cout << whole_expression << endl;
				}
				string num_result = logic_calculator(whole_expression);
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
					if (input_upper.find(" UNION ") == -1)
						chk = 1;
					else
						chk = 3;
					break;
				}
				else if (ele2_upper == "INTO")
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
			tb_name.clear();
			string tbname, tbname_upper;
			do
			{
				is >> tbname;
				if (tbname[tbname.length() - 1] == ',')
					tbname = tbname.substr(0, tbname.length() - 1);
				while (tbname[0] == ' ')
					tbname = tbname.substr(1);
				while (tbname[tbname.length() - 1] == ' ')
					tbname = tbname.substr(0, tbname.length() - 1);
				tbname_upper = "";
				Transform(tbname, tbname_upper);
				if (tbname_upper == "WHERE" || tbname_upper == "GROUP" || tbname[tbname.length() - 1] == ';')
					break;
				else
				{
					tb_name.push_back(tbname);
				}
			} while (tbname_upper != "WHERE" && tbname_upper != "GROUP" && tbname[tbname.length() - 1] != ';');
			//先一直输入的是表名，直到读到"WHERE","GROUP"或者到达末尾为止

			if (tbname[tbname.length() - 1] == ';')
			{
				tbname = tbname.substr(0, tbname.length() - 1);
				tb_name.push_back(tbname);
				has_whereclause = false;
			} //如果是读到语句结尾的话，证明没有where或者group

			if (tb_name.size() == 1) //如果是单表
			{
				string tbname = tb_name[0];
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
				if (has_whereclause)	   //如果有where或者group
				{
					if (tbname_upper == "WHERE") //如果是where，证明是一个输出的语句，按正常情况操作
					{
						condition = "";
						getline(is, condition, ';');
						//cout<<condition<<endl;
						//把whereclause读进condition进行后续操作
						auto outorder = curTb->Select(col_name, condition); //处理，找到符合条件的行存入outorder静态vector
						curTb->show_output_from_select(col_name, outorder); //把符合条件的每一行输出
					}
					else if (tbname_upper == "GROUP") //如果读到了group
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
			else if (tb_name.size() == 0)
				cout << "Invalid input : expect a tablename." << endl;
			else if (tb_name.size() > 2) //不支持三个及以上的表
				cout << "This version doe not support whereclause between three or more tables, please check again." << endl;
			else //两个表
			{
				if (col_name[0] == "*")
				{
					col_name.clear();
					TABLE *ptb = curDb->GetTable(tb_name[0]);
					TABLE *ptb2 = curDb->GetTable(tb_name[1]);
					for (int i = 0; i < ptb->GetColumnName().size(); i++)
					{
						string combine = tb_name[0] + "." + ptb->GetColumnName()[i];
						col_name.push_back(combine);
					}
					for (int i = 0; i < ptb2->GetColumnName().size(); i++)
					{
						string combine = tb_name[1] + "." + ptb2->GetColumnName()[i];
						col_name.push_back(combine);
					}
				}
				for (int i = 0; i < col_name.size(); i++)
					cout << col_name[i] << "\t";
				cout << endl; //表头

				TABLE *ptb = curDb->GetTable(tb_name[0]);
				ptb->UpdateRow();
				std::vector<int> first_order;
				for (int i = 0; i < ptb->GetRowNum(); i++)
				{
					first_order.push_back(i);
				}
				ptb->bubble_sort(first_order); //排序，按照主键顺序一个个排好
				string condition = "true";
				for (int i = 0; i < col_name.size(); i++)
				{
					if (col_name[i].find('.') == -1 && col_name[i] != "*")
					{
						cout << "Invalid input : expect mark '.' between tablename and column name, found in"
							<< "\"" << col_name[i] << "\"" << endl;
						break;
					}
					int pos = col_name[i].find('.');
					std::string coltbname = col_name[i].substr(0, pos);
					col_name[i] = col_name[i].substr(pos + 1);
					col_tb_name.push_back(coltbname);
				} //切割（表名.列名）并分别存于col_tb_name和col_name
				for (int i = 0; i < col_name.size(); i++)
				{
					if (col_tb_name[i] == tb_name[1])
					{
						col_2_name.push_back(col_name[i]);
					}
				} //把属于第二个表的列名字加进向量col_2_name;

				if (has_whereclause) //如果有whereclause
				{
					if (tbname_upper == "WHERE") //如果输入合法
					{
						condition = "";
						getline(is, condition, ';'); //把whereclause读进condition进行后续操作
						while (condition[0] == ' ')
						{
							condition = condition.substr(1);
						}
						TABLE *ptb2 = curDb->GetTable(tb_name[1]);

						for (int i = 0; i < ptb->GetRowNum(); i++) //遍历第一个表里面的每一行
						{
							string c_condition = constify(condition, tb_name, ptb, first_order[i], first_order, col_name, col_tb_name);
							auto outorder = ptb2->Select(col_2_name, c_condition);
							for (int rnd = 0; rnd < outorder.size(); rnd++) //对于第二个表中每一个符合要求的行都要输出一行
							{
								for (int j = 0; j < col_name.size(); j++) //每一列进行输出
								{
									if (col_tb_name[j] == tb_name[0])
									{
										ptb->show_output_from_col(col_name[j], first_order, i);
									}
									else if (col_tb_name[j] == tb_name[1])
									{
										ptb2->show_output_from_col(col_name[j], outorder, rnd);
									}
								}
								cout << endl;
							}
						}
						/*auto outorder = curTb->Select(col_name, condition); //处理，找到符合条件的行存入outorder静态vector
						curTb->show_output_from_select(col_name, outorder); //把符合条件的每一行输出*/
					}
					else //多表只实现where,不再支持group
					{
						cout << "Invalid input : expect \"WHERE\" here but not found." << endl;
					}
				}

				else //没有whereclause，全部输出
				{
					TABLE *ptb = curDb->GetTable(tb_name[0]);
					TABLE *ptb2 = curDb->GetTable(tb_name[1]);
					auto outorder = ptb2->Select(col_2_name, "true");
					for (int i = 0; i < ptb->GetRowNum(); i++) //遍历第一个表里面的每一行
					{
						string c_condition = constify(condition, tb_name, ptb, first_order[i], first_order, col_name, col_tb_name);
						auto outorder = ptb2->Select(col_2_name, condition);
						for (int rnd = 0; rnd < outorder.size(); rnd++) //对于第二个表中每一个符合要求的行都要输出一行
						{
							for (int j = 0; j < col_name.size(); j++) //每一列进行输出
							{
								if (col_tb_name[j] == tb_name[0])
								{
									ptb->show_output_from_col(col_name[j], first_order, i);
								}
								else if (col_tb_name[j] == tb_name[1])
								{
									ptb2->show_output_from_col(col_name[j], outorder, rnd);
								}
							}
							cout << endl;
						}
					}
				}
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
		else if (chk == 3) //SELECT stu_name, ... FROM TBNAME1 UNION (ALL) SELECT stu_name, ... FROM TBNAME2 ORDER BY stu_name;
		{
			string tbname1, tbname2, order_col, buf, buf_upper;
			bool has_all = false;
			is >> tbname1;
			is >> buf >> buf; //读入“UNION”和他后面那个
			Transform(buf, buf_upper);
			if (buf_upper == "ALL")
				has_all = true; //不是ALL就是SELECT
			if (has_all)
				is >> buf; //读入SELECT
			while (buf[buf.length() - 1] == ',')
			{
				buf = buf.substr(0, buf.length() - 1); //去掉逗号
				col_2_name.push_back(buf);
				is >> buf;
			}
			col_2_name.push_back(buf);
			is >> buf >> tbname2 >> buf >> buf >> order_col;
			order_col = order_col.substr(0, order_col.size() - 1);

			/* for (int i = 0; i < col_name.size(); i++)
				cout << col_name[i] << '\t';
			cout << endl;
			for (int i = 0; i < col_2_name.size(); i++)
				cout << col_2_name[i] << '\t';
			cout << endl;
			cout << order_col << endl;*/

			if (col_name[0] != col_2_name[0] || col_name[0] != order_col || col_name.size() != col_2_name.size())
				cout << "Please check your command again!" << endl;
			else
			{
				TABLE *tb1, *tb2;
				tb1 = curDb->DataBaseMap[tbname1];
				tb2 = curDb->DataBaseMap[tbname2];
				tb1->Order_in_Union(col_name, order_col, has_all);
				tb2->Order_in_Union(col_2_name, order_col, has_all);

				/*for (int i = 0; i < tb1->column_to_order.size(); i++)
					cout << tb1->column_to_order[i] << '\t';
				cout << endl;
				for (int i = 0; i < tb2->column_to_order.size(); i++)
					cout << tb2->column_to_order[i] << '\t';
				cout << endl; */

				for (int i = 0; i < col_name.size(); i++)
					cout << col_name[i] << '\t';
				cout << endl;
				int index1 = 0, index2 = 0;
				while (index1 < tb1->column_to_order.size() && index2 < tb2->column_to_order.size())
				{
					bool flag = false; //T表示tb1中较小，F表示tb2中较小
					bool same = false;
					DataType order_type = tb1->GetType(order_col);
					switch (order_type)
					{
					case _INT:
						if (tb1->GetColumn(order_col)->Get_INT_Value(tb1->column_to_order[index1]) < tb2->GetColumn(order_col)->Get_INT_Value(tb2->column_to_order[index2]))
							flag = true;
						if (tb1->GetColumn(order_col)->Get_INT_Value(tb1->column_to_order[index1]) == tb2->GetColumn(order_col)->Get_INT_Value(tb2->column_to_order[index2]))
							same = true;
						break;
					case _DOUBLE:
						if (tb1->GetColumn(order_col)->Get_DOUBLE_Value(tb1->column_to_order[index1]) < tb2->GetColumn(order_col)->Get_DOUBLE_Value(tb2->column_to_order[index2]))
							flag = true;
						if (tb1->GetColumn(order_col)->Get_DOUBLE_Value(tb1->column_to_order[index1]) == tb2->GetColumn(order_col)->Get_DOUBLE_Value(tb2->column_to_order[index2]))
							same = true;
						break;
					case _CHAR:
						if (tb1->GetColumn(order_col)->Get_CHAR_Value(tb1->column_to_order[index1]) < tb2->GetColumn(order_col)->Get_CHAR_Value(tb2->column_to_order[index2]))
							flag = true;
						if (tb1->GetColumn(order_col)->Get_CHAR_Value(tb1->column_to_order[index1]) == tb2->GetColumn(order_col)->Get_CHAR_Value(tb2->column_to_order[index2]))
							same = true;
						break;
					default:
						break;
					}
					if (!has_all && same)
					{
						index1++;
						continue;
					}
					for (int i = 0; i < col_name.size(); i++)
					{
						if (flag)
						{
							COLUMN *pcol = tb1->GetColumn(col_name[i]);
							DataType col_type = tb1->GetType(col_name[i]);
							switch (col_type)
							{
							case _INT:
								cout << pcol->Get_INT_Value(tb1->column_to_order[index1]) << '\t';
								break;
							case _DOUBLE:
								cout << pcol->Get_DOUBLE_Value(tb1->column_to_order[index1]) << '\t';
								break;
							case _CHAR:
								cout << pcol->Get_CHAR_Value(tb1->column_to_order[index1]) << '\t';
								break;
							default:
								break;
							}
						}
						else
						{
							COLUMN *pcol = tb2->GetColumn(col_2_name[i]);
							DataType col_type = tb2->GetType(col_2_name[i]);
							switch (col_type)
							{
							case _INT:
								cout << pcol->Get_INT_Value(tb2->column_to_order[index2]) << '\t';
								break;
							case _DOUBLE:
								cout << pcol->Get_DOUBLE_Value(tb2->column_to_order[index2]) << '\t';
								break;
							case _CHAR:
								cout << pcol->Get_CHAR_Value(tb2->column_to_order[index2]) << '\t';
								break;
							default:
								break;
							}
						}
					}
					cout << endl;
					if (flag)
						index1++;
					else
						index2++;
				}
				if (index1 < tb1->column_to_order.size())
				{
					for (; index1 < tb1->column_to_order.size(); index1++)
					{
						for (int i = 0; i < col_name.size(); i++)
						{
							COLUMN *pcol = tb1->GetColumn(col_name[i]);
							DataType col_type = tb1->GetType(col_name[i]);
							switch (col_type)
							{
							case _INT:
								cout << pcol->Get_INT_Value(tb1->column_to_order[index1]) << '\t';
								break;
							case _DOUBLE:
								cout << pcol->Get_DOUBLE_Value(tb1->column_to_order[index1]) << '\t';
								break;
							case _CHAR:
								cout << pcol->Get_CHAR_Value(tb1->column_to_order[index1]) << '\t';
								break;
							default:
								break;
							}
						}
						cout << endl;
					}
				}
				if (index2 < tb2->column_to_order.size())
				{
					for (; index2 < tb2->column_to_order.size(); index2++)
					{
						for (int i = 0; i < col_name.size(); i++)
						{
							COLUMN *pcol = tb2->GetColumn(col_2_name[i]);
							DataType col_type = tb2->GetType(col_2_name[i]);
							switch (col_type)
							{
							case _INT:
								cout << pcol->Get_INT_Value(tb2->column_to_order[index2]) << '\t';
								break;
							case _DOUBLE:
								cout << pcol->Get_DOUBLE_Value(tb2->column_to_order[index2]) << '\t';
								break;
							case _CHAR:
								cout << pcol->Get_CHAR_Value(tb2->column_to_order[index2]) << '\t';
								break;
							default:
								break;
							}
						}
						cout << endl;
					}
				}
			}
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
			cout << "invalid command :: expect \"DATA\" here but not found." << endl;
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
			cout << "invalid command :: expect \"INTO\" here but not found." << endl;
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
	else if (ele1 == "SAVE")
	{
		string nm, nm0;
		is >> nm0;
		Transform(nm0, nm);
		while (nm[nm.length() - 1] == ';')
			nm = nm.substr(0, nm.length() - 1);
		if (nm == "ALL")
		{
			Allbases.create_allbases_file();
		}
		else if (nm == "DATABASE")
		{
			string dbname;
			is >> dbname;
			if (dbname[dbname.length() - 1] == ';')
				dbname = dbname.substr(0, dbname.length() - 1);
			DATABASE *db_to_save = Allbases.AllBasesMap[dbname];
			db_to_save->create_database_file();
			db_to_save = nullptr;
		}
		else if (nm == "TABLE") //输入格式是SAVE TABLE tbname FROM DATABASE dbname;
		{
			string inter, dbname, tbname;
			is >> tbname;
			is >> inter; //FROM
			is >> inter; //DATABASE
			is >> dbname;
			while (dbname[dbname.length() - 1] == ';')
				dbname = dbname.substr(0, dbname.length() - 1);
			DATABASE *db = Allbases.AllBasesMap[dbname];
			TABLE *tb = db->DataBaseMap[tbname];
			tb->create_table_file(db->get_name());
		}
	}
	else if (ele1 == "RELOAD" || ele1 == "RELOAD;")
	{
		Allbases.load_all_databases();
	}
}

string PARSE::whole_expression_standardize(string whole_expression)
{
	string copy_whole_expression = whole_expression;
	int length = whole_expression.length();
	int i = 1;
	while (i < copy_whole_expression.length() && copy_whole_expression[i] != ';')
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

	/**我希望not或!是和它后面接的数连在一起的**/
	while (copy_whole_expression.find("! ") != -1 || copy_whole_expression.find("NOT ") != -1)
	{
		if (copy_whole_expression.find("! ") != -1)
		{
			copy_whole_expression = copy_whole_expression.substr(0,copy_whole_expression.find("! ")) +" !"+ copy_whole_expression.substr(copy_whole_expression.find("! ") + 2);
			//cout << copy_whole_expression << endl;
		}
		if (copy_whole_expression.find("NOT ") != -1)
		{
			copy_whole_expression = copy_whole_expression.substr(0,copy_whole_expression.find("NOT ")) + "NOT" + copy_whole_expression.substr(copy_whole_expression.find("NOT ") +4);
			//cout << copy_whole_expression << endl;
		}
	}
	return copy_whole_expression.substr(0, i);
}

int PARSE::P(string arithmetic_operator) //制定算术运算符优先级
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
	}
	return to_string(numbers.top());
}

int PARSE::PL(string logic_operator) //制定逻辑运算符优先级
{
	if (logic_operator == "OR" || logic_operator == "XOR")
	{
		return 1;
	}
	if (logic_operator == "AND")
	{
		return 2;
	}
	if (logic_operator == "#")
	{
		return 0;
	}
	return -1;
}

//逻辑计算器
string PARSE::logic_calculator(string &whole_expression)
{
	istringstream s(whole_expression + " # "); //"#"表示算术表达式结束
	std::stack<string> logic_operators;   //运算符栈
	std::stack<int> numbers;				   //数据栈
	logic_operators.push("#");
	string logic;
	bool used = true;
	while (!logic_operators.empty())
	{
		if (s && used)
		{
			s >> logic;
			//cout << logic << endl;
		}
		if (logic != "#" && logic != "AND" && logic != "OR" && logic != "XOR" && logic != "NOT" && logic != "!")
		{
			if (logic.find("NOT")!=-1)
			{
				//cout << logic.substr(logic.find("NOT") + 3) << endl;
				//cout << stoi(logic.substr(logic.find("NOT") + 3)) << endl;
				logic = (stoi(logic.substr(logic.find("NOT") + 3))) > 0 ? "0" : "1";
				//cout << logic << endl;
			}
			if (logic.find("!")!=-1)
			{
				logic = (stoi(logic.substr(logic.find("!") + 1)) > 0) ? "0" : "1";
				//cout << logic << endl;
			}
			numbers.push(stoi(logic));
			continue;
		}

		else
		{
			if (PL(logic_operators.top()) < PL(logic))
			{
				used = true;
				logic_operators.push(logic);
			}
			else
			{
				if (logic == "#" && logic_operators.top() == "#")
				{
					break;
				}
				used = false;
				double number2 = numbers.top();
				numbers.pop();
				double number1 = numbers.top();
				numbers.pop();
				string op = logic_operators.top();
				logic_operators.pop();
				if (to_string(number2) == "NULL" || to_string(number1) == "NULL")
				{
					return "NULL";
				}
				if (op == "AND")
				{
					numbers.push(number1 && number2);
				}
				if (op == "OR")
				{
					numbers.push(number1 || number2);
				}
				if (op == "XOR")
				{
					numbers.push((!number1 || number2) && (number1 || !number2));
				}
			}
		}
	}
	if (numbers.top() == 0)
	{
		return "false";
	}
	else
		return "true";
}



std::string PARSE::constify(std::string condition, const std::vector<std::string> &tbname, TABLE *ptb, int line_num, const std::vector<int> outorder, const std::vector<std::string> &col_name, const std::vector<std::string> &col_tb_name)
{
	string front = "";
	int t = 0;
	int pos = 10000;
	while (pos != -1)
	{
		pos = condition.find('.');
		if (pos == -1)
			break;
		string left = condition.substr(0, pos);
		string right = condition.substr(pos + 1);
		if (left.substr(left.length() - tbname[0].length()) == tbname[0]) //如果是第一个表
		{
			TABLE *ptb = curDb->GetTable(tbname[0]);
			auto COLNM = ptb->GetColumnName();	 //这是一个名为tbname[0]的table里面储存各列名字的vector的常量引用
			for (int i = 0; i < COLNM.size(); i++) //遍历需要查找的每一列
			{
				if (right.length() < COLNM[i].length())
					continue;										//如果长度关系不对就肯定不是这一列
				if (right.substr(0, COLNM[i].length()) == COLNM[i]) //如果这个列符合要求
				{
					string mid = "";
					left = left.substr(0, left.length() - tbname[0].length());
					right = right.substr(COLNM[i].length()); //切割左右字符串，把表名和列名都切掉（点在上面截left和right的时候已经切掉了）

					if (ptb->GetType(COLNM[i]) == _INT)
					{
						mid = to_string(ptb->TableMap[COLNM[i]]->Get_INT_Value(line_num));
					}
					else if (ptb->GetType(COLNM[i]) == _DOUBLE)
					{
						mid = to_string(ptb->TableMap[COLNM[i]]->Get_DOUBLE_Value(line_num));
					}
					else if (ptb->GetType(COLNM[i]) == _CHAR)
					{
						//mid = "   "; //三个空格
						mid.push_back('\"');
						mid += ptb->TableMap[COLNM[i]]->Get_CHAR_Value(line_num);
						mid.push_back('\"');
					}
					condition = left + mid + right;
					ptb = nullptr;
					break;
				}
			}
		}
		else if (left.substr(left.length() - tbname[1].length()) == tbname[1]) //这个时候只需要切掉表名和点就可以了
		{
			left = left.substr(0, left.length() - tbname[1].length());
			condition = left + right;
		}
	}
	/* cout << "after constify: " << condition << endl;*/
	return condition;
}

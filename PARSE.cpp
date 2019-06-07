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
				int val = stoi(*itv);
				curTb->Insert(*ita, val);
			}
			else if (type == 2)
			{
				char val = (*itv)[0];
				curTb->Insert(*ita, val);
			}
			else if (type == 1)
			{
				double val = stod(*itv);
				curTb->Insert(*ita, val);
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
	/*
	else if (ele1 == "SELECT")//SELECT COLNAME FROM TBNAME WHERE .....;
	{
		string ele2;
		is >> ele2;//储存了列名
		string ele3;
		is >> ele3;//FROM
		is >> ele3;//TBNAME
		bool end = false; //判断是否有whereclause语句
		for (int j = 0; ele3[j]; j++)
		{
			if (ele3[j] == ';')
			{
				end = true;
				ele3.erase(j, 1);
				j--;
			}
		}
		for (auto it = curDb->DataBaseMap.begin(); it != curDb->DataBaseMap.end(); it++)
		{
			if (it->first == ele3)
			{
				curTb = it->second;
				break;
			}
			if (it == curDb->DataBaseMap.end())
				cout << "CAN NOT FIND TABLENAME" << ele3 << endl;
		}//利用ele3里面的TBNAME找对应表的指针
		string condition = "true"; //condition缺省为“true”，供无whereclause语句时使用
		if (!end)
		{
			is >> ele3;
			getline(is, condition, ';');
		}//如果有whereclause，用ele3储存"WHERE",condition储存whereclause句子
		auto outorder = curTb->Select(ele2, condition);
		curTb->show_output_from_select(outorder, ele2);
	}*/
	else if (ele1 == "SELECT") //SELECT COLNAME FROM TBNAME WHERE .....;//SELECT COLNAME INTO OUTPUTFILE FROM TBNAME WHERE
	{
		std::vector<std::string> col_name;
		col_name.clear();
		string ele2, ele2_upper;
		int chk = 0;				 //1代表正常SELECT，2代表需要输出到文件（出现“INTO”），3代表。。。（Group by或者sort，大家自行添加）
		bool has_whereclause = true; //判断是否有whereclause语句
		while (chk == 0)
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
			}						   //利用ele3里面的TBNAME找对应表的指针
			string condition = "true"; //condition缺省为“true”，供无whereclause语句时使用
			if (has_whereclause)
			{
				string condition_upper = "";
				is >> condition;//先把后一个要么是where要么是group的串读进condition
				Transform(condition, condition_upper);
				if (condition_upper == "WHERE")//如果是where，证明是一个输出的语句，按正常情况操作
				{
					condition = "";
					getline(is, condition, ';');//把whereclause读进condition进行后续操作
					auto outorder = curTb->Select(col_name, condition);//处理，找到符合条件的行存入outorder静态vector
					curTb->show_output_from_select(col_name, outorder);//把符合条件的每一行输出
				}
				else if(condition_upper == "GROUP")//如果读到了group，那么。。。。。。
				{
					//这部分交给你们了
				}
			}
			else//没有whereclause，全部输出
			{
				auto outorder = curTb->Select(col_name, "true");
				curTb->show_output_from_select(col_name, outorder);
			}
		}
		else if (chk == 2)
		{
			//这部分写输出到文件
		}
	}
}

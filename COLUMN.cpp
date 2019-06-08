#include "COLUMN.h"

using namespace std;

void COLUMN::del(int k)
{ //删除第k+1个数据
	if (k < ColumnData.size())
	{
		ColumnData.erase(ColumnData.begin() + k);
	}
}

bool COLUMN::Get_IsNull(int k)
{
	return ColumnData[k]->isNull();
}

void COLUMN::update(int k, int v)
{ //更新第k+1个数据为v
	dynamic_cast<INT *>(ColumnData[k])->SetValue(v);
}

void COLUMN::update(int k, char v)
{ //更新第k+1个数据为v
	dynamic_cast<CHAR *>(ColumnData[k])->SetValue(v);
}

void COLUMN::update(int k, double v)
{ //更新第k+1个数据为v
	dynamic_cast<DOUBLE *>(ColumnData[k])->SetValue(v);
}

int COLUMN::Get_INT_Value(int k)
{
	return dynamic_cast<INT *>(ColumnData[k])->GetValue();
}

double COLUMN::Get_DOUBLE_Value(int k)
{
	return dynamic_cast<DOUBLE *>(ColumnData[k])->GetValue();
}

char COLUMN::Get_CHAR_Value(int k)
{
	return dynamic_cast<CHAR *>(ColumnData[k])->GetValue();
}

void COLUMN::push_back(int d)
{ //直接加入数据
	pDATA it = new INT(d);
	ColumnData.push_back(it);
}

void COLUMN::push_back_null(int d)
{ //直接加入数据NULL
	pDATA it = new INT();
	it->SetNull();
	ColumnData.push_back(it);
}

void COLUMN::push_back(char d)
{ //直接加入数据
	pDATA it = new CHAR(d);
	ColumnData.push_back(it);
}

void COLUMN::push_back_null(char d)
{ //直接加入数据NULL
	pDATA it = new CHAR();
	it->SetNull();
	ColumnData.push_back(it);
}

void COLUMN::push_back(double d)
{ //直接加入数据
	pDATA it = new DOUBLE(d);
	ColumnData.push_back(it);
}

void COLUMN::push_back_null(double d)
{ //直接加入数据NULL
	pDATA it = new DOUBLE();
	it->SetNull();
	ColumnData.push_back(it);
}

void COLUMN::SetConstraint(ConstraintType ct)
{ //添加限制
	Constraints.push_back(ct);
}

bool COLUMN::isNotNULL()
{ //有无NOT_NULL限制
	for (auto it = Constraints.begin(); it < Constraints.end(); it++)
	{
		if (*it == NOT_NULL)
		{
			return true;
		}
	}
	return false;
}

bool COLUMN::hasDefault()
{
	return DefaultFlag;
}

pDATA COLUMN::GetDefault()
{
	return Default;
}

void COLUMN::SetDefault(int d)
{
	pDATA it = new INT(d);
	Default = it;
}

void COLUMN::SetDefault(char d)
{
	pDATA it = new CHAR(d);
	Default = it;
}

void COLUMN::SetDefault(double d)
{
	pDATA it = new DOUBLE(d);
	Default = it;
}

string COLUMN::GetExtra()
{
	return extra;
}

INT *COLUMN::Get_INT_Data(int k)
{
	return dynamic_cast<INT *>(ColumnData[k]);
}

DOUBLE *COLUMN::Get_DOUBLE_Data(int k)
{
	return dynamic_cast<DOUBLE *>(ColumnData[k]);
}

CHAR *COLUMN::Get_CHAR_Data(int k)
{
	return dynamic_cast<CHAR *>(ColumnData[k]);
}
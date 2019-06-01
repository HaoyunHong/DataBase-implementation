#include "ALLBASES.h"
using namespace std;


DATABASE* ALLBASES::GetDB(string name){//获得数据库指针
	auto it = AllBasesMap.find(name);
	if(it != AllBasesMap.end())
		return it->second;
	else 
		cout<<"Can't find "<<name<<endl;
	return nullptr;
}


void ALLBASES::create(string dbname){//建立新数据库
	DBName.push_back(dbname);
	AllBasesMap.insert(make_pair(dbname,new DATABASE));
	return;
}

void ALLBASES::del(string name){//Drop数据库
	for(auto it=DBName.begin();it!=DBName.end();it++){
		if(*it == name){
			DBName.erase(it);
			if(AllBasesMap[name] != NULL){
				delete AllBasesMap[name];//delete DATABASE*
			}
			AllBasesMap.erase(name);
			break;
		}
	}
	return;
}

void ALLBASES::show(){//列出当前数据库,按字典序
	cout<<"Database"<<endl;
	sort(DBName.begin(),DBName.end());
	for(auto it=DBName.begin();it!=DBName.end();it++){
		std::cout<<*it<<std::endl;
	}
	return;
}

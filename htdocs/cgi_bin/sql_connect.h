#ifndef __SQL_CONNECT__
#define __SQL_CONNECT__

#include<iostream>
#include"mysql.h"
#include<stdlib.h>
#include<stdio.h>
#include<string>
#include<cstring>
#include<vector>
#include<map>
#include"goods.h"

using namespace std;

//char* lib_path="LD_LIBRARY_PATH=mysql的lib库所在的文件夹";

class sql_connecter{
	public:
		sql_connecter();
		//参数：主机、用户名、密码、数据库名
		sql_connecter(const string &host, const string &user, const string &password,
			const string &db);
		~sql_connecter();
		bool begin_connect();//创建数据库连接
		void close_connect();//关闭数据库连接
		void show_info();
		bool create_table(const string &sql);
		int exe_update(const string &sql);
		bool exe_query(const string &sql);
		vector<Goods> exe_select_goods(string &sql);
		map<int,string> exe_select_type(string &sql);
	private:
		void close_result();
	private:
		MYSQL *_mysql_conn;
		MYSQL_RES *_res;
		string _host;
		string _user;
		string _password;
		string _db;
};

#endif

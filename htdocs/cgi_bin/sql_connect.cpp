#include"sql_connect.h"

sql_connecter::sql_connecter()
{
	const string host = "localhost";
	const string user = "fuxiao";
	const string passwd = "pass";
	const string db = "myhttpd";

	this->_host = host;
	this->_user = user;
	this->_password = passwd;
	this->_db = db;
	this->_mysql_conn = mysql_init(NULL);//初始化数据库句柄
	this->_res = NULL;//初始化结果集句柄
}

sql_connecter::sql_connecter(const string &host, const string &user, const string 
		&password, const string &db)
{
	this->_host = host;
	this->_user = user;
	this->_password = password;
	this->_db = db;
	this->_mysql_conn = mysql_init(NULL);//初始化数据库句柄
	this->_res = NULL;//初始化结果集句柄
}

bool sql_connecter::begin_connect()
{
	if( mysql_real_connect(_mysql_conn, _host.c_str(), _user.c_str(), _password.c_str(), _db.c_str(), 3306, NULL, NULL) == NULL){
		cerr<<"connect error\n"<<endl;
		return false;
	}else{
		return true;
	}
}

void sql_connecter::close_connect()
{
	mysql_close(_mysql_conn);
	_mysql_conn = NULL;
}

void sql_connecter::close_result()
{
	if(this->_res != NULL){
		mysql_free_result(_res);
		this->_res = NULL;
	}
}

sql_connecter::~sql_connecter()
{
	if(this->_mysql_conn != NULL){//只有连接成功才进行关闭操作
		close_connect();
	}
	//结果集使用完毕必须free负责会造成内存泄露
	close_result();
}

void sql_connecter::show_info()
{
	std::cout<<mysql_get_client_info()<<std::endl;//输出数据库信息
}

int sql_connecter::exe_update(const string &sql)
{
	int eft_num = -1;
	_res = mysql_store_result(_mysql_conn);
	close_result();

	if( (eft_num = mysql_query(_mysql_conn, sql.c_str())) != 0){
	//	cout<<"update error!\n"<<endl;
		cout<<"<p> database error:"<<mysql_error(_mysql_conn)<<"</p>"<<endl;
		return eft_num;
	}

	_res = mysql_store_result(_mysql_conn);
	eft_num = mysql_affected_rows(_mysql_conn);//get affected rows

	if(_res != NULL){//if null, not query sql
		mysql_free_result(_res);
	}
	//cout<<"update success affected "<<eft_num<<" rows.\n"<<endl;

	return eft_num;
}

bool sql_connecter::create_table(const string &sql)
{
	int flag = exe_update(sql);
	return flag >= 0?true: false;
}

bool sql_connecter::exe_query(const string &sql)
{
	if(mysql_query(_mysql_conn, sql.c_str()) != 0){
		cout<<mysql_error(_mysql_conn)<<endl;
		return false;
	}
	//从MYSQL句柄中提取出查询结果

	_res = mysql_store_result(_mysql_conn);
	int row_num = mysql_num_rows(_res);//查询结果集行数
	cout<<"共查询到："<<row_num<<"行。"<<endl;
	int field_num = mysql_num_fields(_res);//查询结果集列数
	cout<<"共有："<<field_num<<"列。"<<endl;
	
	MYSQL_FIELD *fd = NULL;//打印列名
	for(; fd=mysql_fetch_field(_res);){
		cout<<fd->name<<"\t";
	}
	cout<<endl;

	for(int index=0; index < row_num; ++index){//外层控制打印多少行的信息
		MYSQL_ROW _row = mysql_fetch_row(_res);//获取结果集的一行信息
		if(_row){//如果获取的信息不为空
			int start = 0;
			for(; start < field_num; ++start){
				cout<<_row[start]<<'\t';//依次打印一行中的每一列的信息
			}
			cout<<endl;
		}
	}
	cout<<endl;
	close_result();
	return true;
}

map<int,string> sql_connecter::exe_select_type(string &sql)
{
	map<int,string> type_map;
	if(mysql_query(_mysql_conn, sql.c_str()) != 0){
		cout<<"select_type_error:"<<mysql_error(_mysql_conn)<<endl;
		return type_map;
	}
	
	_res = mysql_store_result(_mysql_conn);
	int row_num = mysql_num_rows(_res);

	for(int row=0; row<row_num; ++row){
		MYSQL_ROW res_row = mysql_fetch_row(_res);
		if(_res != NULL){
			type_map.insert(pair<int,string>(atoi(res_row[0]), res_row[1]));
		}
	}
	return type_map;
}

vector<Goods> sql_connecter::exe_select_goods(string &sql)
{
	vector<Goods> goods_list;//store queryed result list 
	if(mysql_query(_mysql_conn, sql.c_str()) != 0){
		cout<<"mysql_query error:"<<mysql_error(_mysql_conn)<<endl;
		return goods_list;
	}
	//从MYSQL句柄中提取出查询结果

	_res = mysql_store_result(_mysql_conn);
	int row_num = mysql_num_rows(_res);//查询结果集行数
	int col_num = mysql_num_fields(_res);//query filed num

	for(int row=0; row < row_num; ++row){//外层控制打印多少行的信息
		MYSQL_ROW res_row = mysql_fetch_row(_res);//获取结果集的一行信息
		if(res_row){//如果获取的信息不为空
			Goods goods;
			goods.god_id = res_row[0];
			goods.god_name = res_row[1];
			goods.god_type = res_row[2];
			goods.god_price = res_row[3];
			goods.god_size = res_row[4];
			goods_list.push_back(goods);//put this record to goods_list
		}
	}
	return goods_list;
}

/*
int main()
{
	//putenv(lib_path);
	string _host = "localhost";
	string _user = "fuxiao";
	string _passwd = "pass";
	string _db = "myhttpd";

	sql_connecter conn(_host, _user, _passwd, _db);
	conn.begin_connect();//!!!!!!! can't rember this

	why not successed create a table
	string crt_tb_sql("create table goods(\
					    god_id int primary ket auto_increment,\
						god_name varchar(40) not null unique\
					 )");
	int ret = conn.exe_update(crt_tb_sql);
	
	//string insert_sql("insert into goods(god_name) value('small cola');");
	//conn.exe_update(insert_sql);

	string select_sql("select * from goods");
	conn.exe_query(select_sql);

	return 0;
}
*/

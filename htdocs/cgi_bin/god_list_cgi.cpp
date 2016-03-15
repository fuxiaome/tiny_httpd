#include"sql_connect.h"

void print_style()
{
	cout<<"<style type=\"text/css\">\n"<<endl;
    cout<<"body{\n"<<endl;
    cout<<" background-color:#eeffff;\n"<<endl;
    cout<<"}\n";
    cout<<"#godTable{\n";
    cout<<"border:solid;\n";
    cout<<"border-color:5b5c5e;\n";
    cout<<"border-width:1px;\n";
    cout<<"margin-left:120px;\n";
    cout<<"background-color: #eeffff;\n";
    cout<<"font-family:楷体_GB2312;\n";
    cout<<"}\n";
    cout<<".tabRow{\n";
    cout<<"border:dotted;\n";
    cout<<"border-color:#6b6c6e;\n";
    cout<<"border-width:1px;\n";
    cout<<"}\n";
    cout<<"td{\n";
    cout<<"width:70px;\n";
    cout<<"text-align: center;\n";
    cout<<"}\n";
    cout<<"#godTable a:LINK{\n";
    cout<<"color:black;\n";
    cout<<"font-family:楷体_GB2312;\n";
    cout<<"margin-bottom: 6px;\n";
    cout<<"text-decoration: none;\n";
    cout<<"}\n";
    cout<<"#godTable a:HOVER{\n";
    cout<<"color:#cc8291;\n";
    cout<<"font-family:楷体_GB2312;\n";
    cout<<"text-decoration: none;\n";
    cout<<"}\n";
    cout<<" a:VISITED{\n";
    cout<<"color:#918291;\n";
    cout<<"font-size:15px;\n";
    cout<<"font-family:楷体_GB2312;\n";
    cout<<"text-decoration:underline;\n";
    cout<<"}\n";
    cout<<"a:LINK{\n";
    cout<<"color:#6b6c6e;\n";
    cout<<"font-size:medium;\n";
    cout<<"font-family:楷体_GB2312;\n";
    cout<<"margin-bottom: 6px;\n";
    cout<<"text-decoration: none;\n";
    cout<<"}\n";
    cout<<"a:HOVER{\n";
    cout<<"color:#8c93b5;\n";
    cout<<"font-size:17px;\n";
    cout<<"font-family:楷体_GB2312;\n";
    cout<<"text-decoration:underline;\n";
    cout<<"margin-bottom: 6px;\n";
    cout<<"}\n";
    cout<<"#quickMenu a:VISITED{\n";
    cout<<"color:#81838e;\n";
    cout<<"font-size:medium;\n";
    cout<<"font-family:楷体_GB2312;\n";
    cout<<"text-decoration:underline;\n";
    cout<<"margin-bottom: 6px;\n";
    cout<<"}\n";
    cout<<"#oper{\n";
    cout<<"magin-top:20px;\n";
    cout<<"margin-bottom:20px;\n";
    cout<<"margin-left:120px;\n";
    cout<<"}\n";
    cout<<"</style>\n";
}

void print_table(vector<Goods> goods_list)
{
	cout<<"<table id=\"godTable\">\n";
	cout<<"<tr styel=\"text-align:center;border:solid;\">\n";
	cout<<"<td>Id</td><td>Name</td><td>Type</td><td>Price</td>\n";
	cout<<"<td>Size</td><td>Operater</td><td>Delete</td>\n";
	cout<<"</tr>\n";
	for(int i=0; i<goods_list.size(); ++i){
		cout<<"<tr >\n";
		cout<<"<td class=\"tabRow\">"<<goods_list[i].god_id<<"</td>\n";
		cout<<"<td class=\"tabRow\">"<<goods_list[i].god_name<<"</td>\n";
		cout<<"<td class=\"tabRow\">"<<goods_list[i].god_type<<"</td>\n";
		cout<<"<td class=\"tabRow\">"<<goods_list[i].god_price<<"</td>\n";
		cout<<"<td class=\"tabRow\">"<<goods_list[i].god_size<<"</td>\n";
		cout<<"<td class=\"tabRow\"><a href=\"change_god_cgi?god_id="<<goods_list[i].god_id<<"\">Change</a></td>\n";
		cout<<"<td class=\"tabRow\"><a href=\"del_god_cgi?god_id="<<goods_list[i].god_id<<"\">Del</a></td>\n";
		cout<<"</tr>\n";
	}
	cout<<"</table>\n";
}

void add_type(char* data_string)
{
	if(!data_string){
		return;
	}
	char type_name[100];
	sscanf(data_string, "type_name=%s", type_name);

	sql_connecter conn;
	conn.begin_connect();
	string sql("insert into goods_type(godt_name) value('");
	sql += type_name;
	sql += "');";

	if(conn.exe_update(sql) == -1){
		cout<<"<p>"<<type_name<<" insert failed! </p>\n"<<endl;	
	}else{
		cout<<"<p>"<<type_name<<" insert successed! </p>\n"<<endl;	
	}
}

string get_select_sql(char *data_string)
{
	string sql;
	if(!data_string){
		sql += "select god_id,god_name,godt_name,god_price,god_size from goods inner join goods_type on god_type=godt_id;";
	}else{
		char god_id[40] = {'\0'};
		char god_name[40] = {'\0'};
		char god_type[40] = {'\0'};
		char god_price_l[40] = {'\0'};
		char god_price_h[40] = {'\0'};
		char god_size[40] = {'\0'};
		for(int i=0,len=strlen(data_string); i<len; ++i){
			if(data_string[i]=='&' || data_string[i]=='='){
				data_string[i] = ' ';
			}
		}
		sscanf(data_string,"%*s%s%*s%s%*s%s%*s%s%*s%s%*s%s", god_id, god_name, god_type, god_price_l, god_price_h, god_size);
		sql += "select god_id,god_name,godt_name,god_price,god_size from goods inner join goods_type on god_type=godt_id where 1=1 "; 
		if(strlen(god_id) > 0 && strcmp(god_id, "xx")!=0){
			sql += " and god_id=";
			sql += god_id;
		}
		if(strlen(god_name) > 0 && strcmp(god_name, "xx")!=0){
			sql += " and god_name like '%";
			sql += god_name;
			sql += "%' ";
		}
		if(strlen(god_type) > 0 && strcmp(god_type, "-1")!=0){
			sql += " and god_type=";
			sql += god_type;
		}
		if(strlen(god_price_l) > 0 && strlen(god_price_h)>0 && strcmp(god_price_l, "xx.xx")!=0 && strcmp(god_price_h, "xx.xx")!=0){
			sql += " and god_price between ";
			sql += god_price_l;
			sql += " and ";
			sql += god_price_h;
		}else if(strlen(god_price_l) > 0 && strcmp(god_price_l, "xx.xx")!=0){
			sql += " and god_price >= ";
			sql += god_price_l;
		}else if(strlen(god_price_h) > 0 && strcmp(god_price_h, "xx.xx")!=0){
			sql += " and god_price <= ";
			sql += god_price_h;
		}
		if(strlen(god_size) > 0 && strcmp(god_size, "any")!=0){
			sql += " and god_size = '";
			sql += god_size;
			sql += "' ;";
		}
	}
	return sql;
}

int main()
{
	char method[1024];
	char post_data[1024];
	int content_length = 0;
	vector<Goods> goods_list;
	sql_connecter conn;
	conn.begin_connect();

	memset(method, 0 ,sizeof(method));
	memset(post_data, 0 ,sizeof(post_data));

	cout<<"<html>\n"<<endl;
	cout<<"<head><title>Goods list</title>\n"<<endl;
	print_style();
	cout<<"</head>\n";
	cout<<"<body>\n"<<endl;
	strcpy(method, getenv("REQUEST_METHOD"));

	cout<<"<div id=\"oper\">\n";
	cout<<"-<a href=\"add_god_show_cgi\" >Add new goods</a>-\n";
	cout<<"-<a href=\"find_god_cgi\">Find goods</a>-\n";
	cout<<"-<a href=\"../index.html\">Back index</a>-\n";
	cout<<"</div>\n";
	if(strcasecmp("GET", method) == 0){
		string sql = get_select_sql(NULL);
		goods_list = conn.exe_select_goods(sql);
	}else if(strcasecmp("POST", method) == 0){
		content_length = atoi(getenv("CONTENT_LENGTH"));
		int i = 0;
		for(; i<content_length; ++i){
			read(0, &post_data[i], 1);
		}	
		if(strstr(post_data, "god_size") == NULL){
			strcat(post_data, "&god_size=any");
		}
		post_data[strlen(post_data)] = '\0';
	
		string sql = get_select_sql(post_data);
		goods_list = conn.exe_select_goods(sql);
	}else{
		return 1;
	}
	if(goods_list.size() > 0){
		print_table(goods_list);
	}else{
		cout<<"<h3 style=\"margin-top:35px;margin-left:120px\">"<<"Can't find suitable record!</h3>\n";
		cout<<"<p style=\"margin-left:170px\">-<a href=\"god_list_cgi\" >Back to goods list</a>-</p>\n";
	}
	cout<<"</body>\n"<<"</html>\n"<<endl;
	
	return 0;
}

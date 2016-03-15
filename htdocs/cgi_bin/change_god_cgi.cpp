#include"sql_connect.h"

void print_type(map<int,string> type_map)
{
	map<int,string>::iterator it;
	for(it=type_map.begin(); it!=type_map.end(); it++){
		cout<<"<option value=\""<<it->first<<"\">"<<it->second<<"</option>\n";
	}
}

void print_table(map<int,string> type_map, Goods &goods)
{
	map<int, string>::iterator map_it;
	map_it = type_map.find(atoi(goods.god_type.c_str()));
	int type_id = map_it->first;
	string type_name = map_it->second;
	type_map.erase(map_it);
	cout<<"<table id=\"addGodTab\">\n";
	cout<<"<caption style=\"font-size: 17px\">Current goods information</caption>\n";
	cout<<"<tr>\n";
	cout<<"<td>Id:</td><td><input type=\"text\" readonly=\"readonly\" name=\"god_id\" value=\""<<goods.god_id<<"\"/></td>\n";
	cout<<"</tr>\n";
	cout<<"<tr>\n";
	cout<<"<td>Name:</td><td><input type=\"text\" name=\"god_name\" value=\""<<goods.god_name<<"\"/></td>\n";
	cout<<"</tr>\n";
	cout<<"<tr>\n";
	cout<<"<td>Type:</td>\n";
	cout<<"<td>\n";
	cout<<"<select name=\"god_type\">\n";
	cout<<"<option value=\""<<type_id<<"\" selected=\"selected\">"<<type_name<<"</option>\n";
	print_type(type_map);//dynamic get goods_type
	cout<<"</select>\n";
	cout<<"</td>\n";
	cout<<"</tr>\n";
	cout<<"<tr>\n";
	cout<<"<td>Price:</td><td><input type=\"text\" name=\"god_price\" value=\""<<goods.god_price<<"\"/></td>\n";
	cout<<"</tr>\n";
	cout<<"<tr>\n";
	cout<<"<td>Size:</td><td>\n";
	cout<<"<input type=\"radio\" name=\"god_size\" "<<((strcmp("big",goods.god_size.c_str())==0?"checked=\"checked\"":""))<<" value=\"big\"/>Big\n";
	cout<<"<input type=\"radio\" name=\"god_size\" "<<((strcmp("middle",goods.god_size.c_str())==0?"checked=\"checked\"":""))<<" value=\"middle\"/>Middle\n";
	cout<<"<input type=\"radio\" name=\"god_size\" "<<((strcmp("small",goods.god_size.c_str())==0?"checked=\"checked\"":""))<<" value=\"small\"/>Small\n";
	cout<<"</td>\n";
	cout<<"</tr>\n";
	cout<<"<tr>\n";
	cout<<"<td><input type=\"submit\" value=\"change\"/></td>\n";
	cout<<"<td><input type=\"reset\" value =\"reset\"/></td>\n";
	cout<<"</tr>\n";
	cout<<"</table>\n";
}

void change_god(char *data_string)
{	
	if(!data_string){
		return;
	}
	char goods_id[40];
	char goods_name[40];
	char goods_type[40];
	char goods_price[40];
	char goods_size[40];

	for(int i=0,len=strlen(data_string); i<len; ++i){
		if(data_string[i]=='&' || data_string[i]=='='){
			data_string[i] = ' ';
		}
	}
	sscanf(data_string,"%*s%s%*s%s%*s%s%*s%s%*s%s", goods_id, goods_name,goods_type,goods_price,goods_size);

	sql_connecter conn;
	conn.begin_connect();
	string sql("update goods set god_name='");
	sql += goods_name;
	sql += "' , god_type=";
	sql += goods_type;
	sql += " , god_price=";
	sql += goods_price; 
	sql += " , god_size='";
	sql += goods_size;
	sql += "' where god_id=";
	sql += goods_id;
	sql += ";";

	write(2,sql.c_str(),sql.size());
	cout<<"<script language=\"javascript\">\n";
	if(conn.exe_update(sql) == -1){
		cout<<"alert(\"change failed!\")\n";
	}else{
		cout<<"alert(\"change success!\")\n";
	}
	cout<<"window.location.href=\"god_list_cgi\"\n";
	cout<<"</script>\n";
}

int main()
{
	char query_string[1024];
	char method[1024];
	map<int,string> type_map;

	sql_connecter conn;
	conn.begin_connect();

	memset(method, 0, sizeof(query_string));
	memset(method, 0 ,sizeof(method));
	
	cout<<"<html>\n"<<endl;
	cout<<"<head><title>Change goods information</title>\n"<<endl;	

	strcpy(method, getenv("REQUEST_METHOD"));
	write(2, method, strlen(method));

	if(strcasecmp("GET", method) == 0){
		cout<<"<body>\n"<<endl;
		cout<<"<style type=\"text/css\">\n";
		cout<<"body{\n";
		cout<<"background-color: #eeffff;\n";
		cout<<"}\n";
		cout<<"#addGodTab{\n";
		cout<<"margin-left:120px;\n";
		cout<<"background-color: #eeffff;\n";
		cout<<"}\n";
		cout<<"</style>\n";
		cout<<"</head>\n";
		cout<<"<body><p><a href=\"../index.html\">Back index</a></p>\n"<<endl;
		string map_sql("select * from goods_type;");
		type_map = conn.exe_select_type(map_sql);
		cout<<"<form action=\"change_god_cgi\" method=\"POST\">\n";
		strcpy(query_string, getenv("QUERY_STRING"));
		char god_id[40] = {'\0'};
		sscanf(query_string, "god_id=%s", god_id);
		string sql = "select god_id,god_name,god_type,god_price,god_size from goods where god_id=";
		sql += god_id;
		sql += ";";
		vector<Goods> god_list = conn.exe_select_goods(sql);
		print_table(type_map, god_list[0]);
		cout<<"</form>\n</body>\n";
	}else if(strcasecmp("POST", method) == 0){
		int content_length = atoi(getenv("CONTENT_LENGTH"));
		char post_data[1024] = {'\0'};
		int i = 0;
		for(; i<content_length; ++i){
			read(0, &post_data[i], 1);
		}
		post_data[i] = '\0';
		write(2, post_data, strlen(post_data));
		change_god(post_data);
		cout<<"</head>\n";
	}else{
		return 1;
	}

	cout<<"</html>\n"<<endl;
	
	return 0;
}

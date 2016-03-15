#include"sql_connect.h"

void print_type(map<int,string> type_map)
{
	map<int,string>::iterator it;
	for(it=type_map.begin(); it!=type_map.end(); it++){
		cout<<"<option value=\""<<it->first<<"\">"<<it->second<<"</option>\n";
	}
}

void print_table(map<int,string> type_map)
{
	
	cout<<"<table id=\"addGodTab\">\n";
	cout<<"<caption style=\"font-size: 17px\">Please input goods information</caption>\n";
	cout<<"<tr>\n";
	cout<<"<td>Name:</td><td><input type=\"text\" name=\"god_name\"/></td>\n";
	cout<<"</tr>\n";
	cout<<"<tr>\n";
	cout<<"<td>Type:</td>\n";
	cout<<"<td>\n";
	cout<<"<select name=\"god_type\">\n";
	cout<<"<option value=\"-1\" selected=\"selected\">-please choice-</option>\n";
	print_type(type_map);//dynamic get goods_type
	cout<<"</select>\n";
	cout<<"</td>\n";
	cout<<"</tr>\n";
	cout<<"<tr>\n";
	cout<<"<td>Price:</td><td><input type=\"text\" name=\"god_price\"/></td>\n";
	cout<<"</tr>\n";
	cout<<"<tr>\n";
	cout<<"<td>Size:</td><td>\n";
	cout<<"<input type=\"radio\" name=\"god_size\" value=\"big\"/>Big\n";
	cout<<"<input type=\"radio\" name=\"god_size\" value=\"middle\"/>Middle\n";
	cout<<"<input type=\"radio\" name=\"god_size\" value=\"small\"/>Small\n";
	cout<<"</td>\n";
	cout<<"</tr>\n";
	cout<<"<tr>\n";
	cout<<"<td><input type=\"submit\" value=\"insert\"/></td>\n";
	cout<<"<td><input type=\"reset\" value =\"reset\"/></td>\n";
	cout<<"</tr>\n";
	cout<<"</table>\n";
}

int main()
{
	char method[1024];
	map<int,string> type_map;
	sql_connecter conn;
	conn.begin_connect();

	memset(method, 0 ,sizeof(method));
	
	cout<<"<html>\n"<<endl;
	cout<<"<head><title>Add new goods</title>\n"<<endl;	
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
	cout<<"<body>\n"<<endl;

	cout<<"<p><a href=\"../index.html\">Back index</a></p>\n"<<endl;
	strcpy(method, getenv("REQUEST_METHOD"));
	
	cout<<"<form action=\"add_god_cgi\" method=\"POST\">\n";
	string sql("select * from goods_type;");
	type_map = conn.exe_select_type(sql);
	if(strcasecmp("GET", method) == 0){
		print_table(type_map);
	}else{
		return 1;
	}
	cout<<"</form>\n";

	cout<<"</body>\n"<<"</html>\n"<<endl;
	
	return 0;
}

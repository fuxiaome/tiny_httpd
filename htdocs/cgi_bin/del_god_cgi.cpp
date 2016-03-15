#include"sql_connect.h"

void del_goods(char* data_string)
{
	if(!data_string){
		return;
	}
	char god_id[100];
	sscanf(data_string, "god_id=%s", god_id);

	sql_connecter conn;
	conn.begin_connect();
	string sql("delete from goods where god_id=");
	sql += god_id;
	sql += ";";

	cout<<"<script language=\"javascript\">\n";
	if(conn.exe_update(sql) == -1){
		cout<<"alert(\"delete failed!\")\n";
	}else{
		cout<<"alert(\"delete success!\")\n";
	}
	cout<<"window.location.href=\"god_list_cgi\"\n";
	cout<<"</script>\n";
}

int main()
{
	char method[1024];
	char query_string[1024];
	int content_length = 0;
	memset(method, 0 ,sizeof(method));
	
	cout<<"<html>\n"<<endl;
	cout<<"<head><title>Delete a goods</title>\n"<<endl;
	strcpy(method, getenv("REQUEST_METHOD"));

	if(strcasecmp("GET", method) == 0){
		strcpy(query_string ,getenv("QUERY_STRING"));
		del_goods(query_string);
	}else{
		return 1;
	}

	cout<<"</head>\n"<<"</html>\n"<<endl;
	
	return 0;
}

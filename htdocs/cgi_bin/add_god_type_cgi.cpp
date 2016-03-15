#include"sql_connect.h"

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

	cout<<"<script language=\"javascript\">\n";
	if(conn.exe_update(sql) == -1){
		cout<<"alert(\"insert new type failed!\")\n";
	}else{
		cout<<"alert(\"insert new type success!\")\n";
	}
	cout<<"window.location.href=\"../index.html\"\n";
	cout<<"</script>\n";
}

int main()
{
	char method[1024];
	char post_data[1024];
	int content_length = 0;
	memset(method, 0 ,sizeof(method));
	
	cout<<"<html>\n"<<endl;
	cout<<"<head><title>Insert new type result</title>\n"<<endl;
	strcpy(method, getenv("REQUEST_METHOD"));

	if(strcasecmp("POST", method) == 0){
		content_length = atoi(getenv("CONTENT_LENGTH"));
		int i = 0;
		for(; i<content_length; ++i){
			read(0, &post_data[i], 1);
		}
		post_data[i] = '\0';
		cout<<"<p>Post data is:"<<post_data<<endl;
		add_type(post_data);
	}else{
		return 1;
	}

	cout<<"</head>\n"<<"</html>\n"<<endl;
	
	return 0;
}

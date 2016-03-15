#include"sql_connect.h"

void add_goods(char* data_string)
{
	if(!data_string){
		return;
	}
	char goods_name[40];
	char goods_type[40];
	char goods_price[40];
	char goods_size[40];

	for(int i=0,len=strlen(data_string); i<len; ++i){
		if(data_string[i]=='&' || data_string[i]=='='){
			data_string[i] = ' ';
		}
	}
	sscanf(data_string,"%*s%s%*s%s%*s%s%*s%s", goods_name,goods_type,goods_price,goods_size);

	sql_connecter conn;
	conn.begin_connect();
	string sql("insert into goods(god_name,god_type,god_price,god_size) value('");
	sql += goods_name;
	sql += "',";
	sql += goods_type;
	sql += ",";
	sql += goods_price; 
	sql += ",'";
	sql += goods_size;
	sql += "');";

	cout<<"<script language=\"javascript\">\n";
	if(conn.exe_update(sql) == -1){
		cout<<"alert(\"insert failed!\")\n";
	}else{
		cout<<"alert(\"insert success!\")\n";
	}
	cout<<"window.location.href=\"god_list_cgi\"\n";
	cout<<"</script>\n";
}

int main()
{
	char method[1024];
	char post_data[1024];
	int content_length = 0;
	memset(method, 0 ,sizeof(method));
	
	cout<<"<html>\n"<<endl;
	cout<<"<head><title>Insert new goods result</title>\n"<<endl;
	strcpy(method, getenv("REQUEST_METHOD"));

	if(strcasecmp("POST", method) == 0){
		content_length = atoi(getenv("CONTENT_LENGTH"));
		int i = 0;
		for(; i<content_length; ++i){
			read(0, &post_data[i], 1);
		}
		post_data[i] = '\0';
		cout<<"<p>Post data is:"<<post_data<<endl;
		add_goods(post_data);
	}else{
		return 1;
	}
	cout<<"</head>\n";
	cout<<"<p><a href=\"../index.html\">Back index</a></p>"<<endl;
	cout<<"</body>\n"<<"</html>\n"<<endl;
	
	return 0;
}

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main()
{
	char method[1024];
	char query_string[1024];
	char post_data[4096];
	int content_length = 0;
	memset(method, 0 ,sizeof(method));
	memset(query_sting, 0, sizeof(query_sting));
	
	printf("<html>\n");
	printf("<h1>Hello World!</h1><br/>\n");
	strcpy(method, getenv("REUEST_METHOD"));
	printf("<p>method:%s</p><br/>", method);
	
	if(strcasecmp("GET", method) == 0){
		strcpy(query_string, getenv("QUERY_STRING"));
		printf("<p>query_string:%s</p><br/>", query_string);
	}else if(strcasecmp("POST", method) == 0){
		content_length = atoi(getenv("CONTENT_LENGTH"));
		int i = 0;
		for(; i<content_length; ++i){
			read(0, &post_data[i], 1);
		}
		post_data[i] = '\0';
		printf("<p>Post data is:%s</p>", post_data);
	}else{
		return 1;
	}
	
	printf("</html>\n");
	
	return 0;
}
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void math_add(char* data_string)
{
	if(!data_string){
		return;
	}
	char *data1 = NULL;
	char *data2 = NULL;
	char *start = data_string;
	while(*start != '\0'){
		if(*start == '=' && data1 == NULL){
			data1 = start+1;
			start++;
			continue;
		}
		if(*start == '&'){
			*start = '\0';
		}
		if(*start == '=' && data1 != NULL){
			data2 = start+1;
			break;
		}
		start++;
	}
	int idata1 = atoi(data1);
	int idata2 = atoi(data2);
	int add_res = idata1 + idata2;

	printf("<script language=\"javascript\">\n");
	printf("alert(\"add result=%d\")\n", add_res);
	printf("window.location.href=\"../index.html\"\n");
	printf("</script>\n");
}

int main()
{
	char method[1024];
	char query_string[1024];
	char post_data[1024];
	int content_length = 0;
	memset(method, 0 ,sizeof(method));
	memset(query_string, 0, sizeof(query_string));
	
	printf("<html>\n");
	printf("<head><title>MATH CAL</title>\n");
	strcpy(method, getenv("REQUEST_METHOD"));
	write(2, method, strlen(method));
	if(strcasecmp("GET", method) == 0){
		strcpy(query_string, getenv("QUERY_STRING"));
		write(2, query_string, strlen(query_string));
		math_add(query_string);//data1=xx&data2=xx
	}else if(strcasecmp("POST", method) == 0){
		content_length = atoi(getenv("CONTENT_LENGTH"));
		int i = 0;
		for(; i<content_length; ++i){
			read(0, &post_data[i], 1);
		}
		post_data[i] = '\0';
		math_add(post_data);
	}else{
		return 1;
	}
	
	printf("</head>\n");
	printf("</html>\n");
	
	return 0;
}

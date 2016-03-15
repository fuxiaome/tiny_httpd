CC=gcc
WORK_PATH=$(shell pwd)
SQL_INCLUDE=-I${WORK_PATH}/mysql_lib/include -I .
SQL_SRC=sql_connect.cpp
SQL_LIB= -L${WORK_PATH}/mysql_lib/lib
SQL_FLAG=-lmysqlclient

.PHONY:all
all:${SQL_BIN} math_cgi debug_cgi add_god_type_cgi god_list_cgi add_god_show_cgi add_god_cgi del_god_cgi find_god_cgi change_god_cgi
	
math_cgi:math_cgi.c
	$(CC) -o $@ $^
debug_cgi:debug_cgi.c
	$(CC) -o $@ $^
add_god_type_cgi:add_god_type_cgi.cpp ${SQL_SRC}
	g++ -o $@ $^ ${SQL_INCLUDE} ${SQL_LIB} ${SQL_FLAG}
god_list_cgi:god_list_cgi.cpp ${SQL_SRC}
	g++ -o $@ $^ ${SQL_INCLUDE} ${SQL_LIB} ${SQL_FLAG}
add_god_show_cgi:add_god_show_cgi.cpp ${SQL_SRC}
	g++ -o $@ $^ ${SQL_INCLUDE} ${SQL_LIB} ${SQL_FLAG}
add_god_cgi:add_god_cgi.cpp ${SQL_SRC}
	g++ -o $@ $^ ${SQL_INCLUDE} ${SQL_LIB} ${SQL_FLAG}
del_god_cgi:del_god_cgi.cpp ${SQL_SRC}
	g++ -o $@ $^ ${SQL_INCLUDE} ${SQL_LIB} ${SQL_FLAG}
find_god_cgi:find_god_cgi.cpp ${SQL_SRC}
	g++ -o $@ $^ ${SQL_INCLUDE} ${SQL_LIB} ${SQL_FLAG}
change_god_cgi:change_god_cgi.cpp ${SQL_SRC}
	g++ -o $@ $^ ${SQL_INCLUDE} ${SQL_LIB} ${SQL_FLAG}

.PHONY:output
output:
	cp add_god_type_cgi ../output/htdocs/cgi_bin
	cp god_list_cgi ../output/htdocs/cgi_bin
	cp add_god_show_cgi ../output/htdocs/cgi_bin
	cp add_god_cgi ../output/htdocs/cgi_bin
	cp del_god_cgi ../output/htdocs/cgi_bin
	cp find_god_cgi ../output/htdocs/cgi_bin
	cp change_god_cgi ../output/htdocs/cgi_bin

.PHONY:clean
clean:
	rm -rf math_cgi
	rm -rf debug_cgi
	rm -rf add_god_type_cgi
	rm -rf god_list_cgi
	rm -rf add_god_show_cgi
	rm -rf add_god_cgi
	rm -rf del_god_cgi
	rm -rf find_god_cgi
	rm -rf change_god_cgi
#expot LD_LIBRARY_PATH=pwd/mysql_lib/lib
#unset ..

PWD=$(shell pwd)
CGI_PATH=$(PWD)/htdocs/cgi_bin
SER_BIN=httpd
CLI_BIN=demo_client
SER_SRC=httpd.c
CLI_SRC=demo_client.c
CC=gcc
FLAGS=-w -o #-w用来屏蔽warting
LDFLAGS=-pthread #-static

.PHONY:all
all:$(SER_BIN) $(CLI_BIN) cgi

$(SER_BIN):$(SER_SRC)
	$(CC) $(FLAGS) $@ $^ $(LDFLAGS) -D _DEBUG_
$(CLI_BIN):$(CLI_SRC)
	$(CC) $(FLAGS) $@ $^ $(LDFLAGS)

.PHONY:cgi
cgi:
	for name in `echo $(CGI_PATH)` ;\
		do\
			cd $$name;\
			make;\
			cd -;\
		done

.PHONY:output
output:all
	mkdir output
	mkdir -p output/cgi_bin
	cp httpd output
	cp -rf htdocs/* output
	mkdir -p output/htdocs/cgi_bin
	cp -rf conf output
	cp -rf log output
	cp start.sh output
	
	for name in `echo $(CGI_PATH)`;\
	do\
		cd $$name;\
		make output;\
		cd -;\
	done
		
.PHONY:clean
clean:
	rm -f $(SER_BIN) $(CLI_BIN) output
	for name in echo `$(CGI_PATH)`
		do\
			cd $$name;\
			make clean;\
			cd -;\
		done
	
	

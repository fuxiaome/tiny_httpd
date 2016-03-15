#!/bin/bash
WORK_PATH=$(pwd)
HTTPD=${WORK_PATH}/httpd
HTTPD_PORT='8080'
HTTPD_IP='127.0.0.1'

echo 'httpd begin start...'
export LD_LIBRARY_PATH=${WORK_PATH}/htdocs/cgi_bin/mysql_lib/lib
${HTTPD}  ${HTTPD_IP}  ${HTTPD_PORT}
echo 'httpd start done...'

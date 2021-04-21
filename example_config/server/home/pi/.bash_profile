#
# trap deadly signals
#
trap "" SIGHUP SIGQUIT
trap "sh $HOME/.glogout" SIGTERM EXIT
umask 022

if [ "$PS1" ]; then
	declare -x PROMPT_COMMAND='PS1="$HOSTNAME:($USER) `pwd` >"'
fi

# If the user has her own init file, then use that one, else use the
# canonical one.
declare -x PATH=$PATH:$HOME/bin:/usr/libexec/indimail
if [ -f ~/.bashrc ]; then
	source ~/.bashrc
else
if [ -f ${default_dir}Bashrc ]; then
	source ${default_dir}Bashrc;
fi
fi
declare -x LESS=-e
declare -x SIMPLE_BACKUP_SUFFIX='.BAK'
USERNAME=`whoami`
ENV=$HOME/.bashrc
export USERNAME PATH

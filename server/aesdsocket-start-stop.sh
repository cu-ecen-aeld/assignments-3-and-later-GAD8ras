#!/bin/sh
echo $1
case "$1" in
	start)
		echo "Starting aesdsocket"
		start-stop-daemon -S -n aesdsocket -a /usr/bin/aesdsocket
	;;
	stop)
		echo "Stoping the daemon"
		start-stop-daemon -K -n aesdsocket
	;;
	*)
		echo "Usage: $0 {start|stop}"
	exit 1
esac
	

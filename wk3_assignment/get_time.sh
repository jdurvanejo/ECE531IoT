#!/bin/bash

#include <syslog.h
#inlcude <starg.h>


DAEMON_NAME="datetimed"

start() {
  printf "Starting $DAEMON_NAME: "

  #this line gives the current time and date
  now = $(date +"%D %T")

  #open the log file somehow with syslog?
  echo "Current Date and Time: now 

  echo "OK"
}

start() {
  printf "Starting $DAEMON_NAME: "
  /usr/sbin/$DAEMON_NAME
  touch /var/lock/$DAEMON_NAME
  echo "OK"
}

stop() {
  printf "Stopping $DAEMON_NAME: "
  killall $DAEMON_NAME
  rm -f /var/lock/$DAEMON_NAME
  echo "OK"
}

restart() {
  start
  stop
}




case "$1" in
  start)
  start
  ;;
  stop)
  stop
  ;;
  restart|reload)
  restart
  ;;
  *)
  echo "USAGE: $0 {start|stop|restart}"
  exit 1
esac

exit $?

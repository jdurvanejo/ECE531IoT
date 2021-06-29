#!/bin/sh

DAEMON_NAME="TEST DAEMON"

start() {
  printf "Starting $DAEMON_NAME: "
  echo "OK"
}

stop() {
  printf "Stopping $DAEMON_NAME: "
  echo "OK"
}

restart() {
  stop
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

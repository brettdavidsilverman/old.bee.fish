#!/bin/bash
### BEGIN INIT INFO
# Provides:          https-server 
# Required-Start:    $local_fs $network $named $time $syslog
# Required-Stop:     $local_fs $network $named $time $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Description:       Runs http(s)-server 
### END INIT INFO


SERVICE="https-server"

status() {

   if su -c "pgrep -x $SERVICE" - "bee" >/dev/null
   then
      echo "$SERVICE is running"
      exit 0
   else
      exit 1
      echo "$SERVICE stopped"
   fi

}

start() {

   su -c "~/bee.fish/cgi/socket/start" - "bee"

}

stop() {

  
   su -c "~/bee.fish/cgi/socket/stop" - "bee"
  
}

install() {
  update-rc.d https-server defaults
}

uninstall() {
  update-rc.d -f https-server remove
}

case "$1" in
  start)
    start
    ;;
  stop)
    stop
    ;;
  install)
    install
    ;;
  uninstall)
    uninstall
    ;;
  restart)
    stop
    start
    ;;
  status)
    status
    ;;
  *)
    echo "Usage: $0 {start|stop|restart|status|install|uninstall}"
esac

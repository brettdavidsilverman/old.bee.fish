#!/bin/sh
### BEGIN INIT INFO
# Provides:          https-server 
# Required-Start:    $local_fs $network $named $time $syslog
# Required-Stop:     $local_fs $network $named $time $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Description:       Runs http(s)-server 
### END INIT INFO

start() {

  cd /home/bee/bee.fish/cgi/socket
  ./start

}

stop() {

  /home/bee/bee.fish/cgi/socket/stop
  
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
  *)
    echo "Usage: $0 {start|stop|restart|install|uninstall}"
esac

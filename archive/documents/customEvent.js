function CustomEvent() {
    var listeners = [];
    var nextIdentifier = 0;
    this.addListener = function(listener) {
        listener.identifier = "_" + nextIdentifier++;;
        listeners[listener.identifier] = listener;
    } 

    this.removeListener = function(listener) {
        delete listeners[listener.identifier];
    }

    this.dispatch = function(event) {
        var cancelled = false;
        for (var listener in listeners) {
             var result = listeners[listener](event);
             if (!result)
                 cancelled = true
        }
        return cancelled;
    }
}
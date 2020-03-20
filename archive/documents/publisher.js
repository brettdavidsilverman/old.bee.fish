function Publisher(instance) {
    var instance = instance;
    var subscribers = [];
    this.subscribe = function(subscriber) {
        subscribers.push(subscriber);
    }

    this.publish(obj) {
        for (var subscriber in subscribers) {
           subscribers[subscriber].call(instance, obj);
        }
    }
}

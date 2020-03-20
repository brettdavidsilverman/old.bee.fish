Storage.prototype.clear = function(type) {
    var localName = type + ":";
    for (var property in this) {
        if (property.indexOf(localName) == 0)
            delete this[property];
    }
}

Storage.prototype.clearAll = function() {
    for (var property in this) {
        if (this.hasOwnProperty(property))
            delete this[property];
    }
}
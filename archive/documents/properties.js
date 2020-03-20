Object.prototype.properties = function() {
    var properties = new Array();
    for (var property in this) {
        var value = property;
        var type = typeof(this[property]);
        value += ": " + type;
        if (type == "number")
            value += "(" + this[property] + ")";
        properties.push(value);
    }
    properties.sort();
    alert(properties.join("\n"));
}
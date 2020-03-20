Object.prototype.properties = function() {
    var properties = new Array();
    for (var property in this) {
        properties.push(property + ": " + typeof(this[property]));
    }
    properties.sort();
    alert(properties.join("\n"));
}
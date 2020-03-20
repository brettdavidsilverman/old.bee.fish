function alertProperties(obj) {
    var properties = new Array();
    for (var property in obj) {
        var value = property;
        value += "(" + this[property] + ")";
        properties.push(value);
    }
    properties.sort();
    alert(properties.join("\n"));
}
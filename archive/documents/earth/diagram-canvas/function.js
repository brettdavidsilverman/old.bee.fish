function Function(name, diagrams) {
    this.oldCoordinates = new Array();
    this.subdiagrams = new Subdiagrams(this, diagrams);
    if (name != undefined)
        this.name = name;
    else
        this.name = "function";
    this.width = function() {
        return this.radius() * 2;
    }

    this.height = this.width;
    this.fontSize = 10;
    this.font = this.fontSize + "px Courier New";

    this.radius = function() {
        var context = this.context();
        context.save();
        context.font = this.font;

        var width = context.measureText(this.name).width;
        var height = this.fontSize;
        var radius;
        if (height > width)
            radius = height / 2;
        else
            radius = width / 2;
        context.restore();
        return radius;
    }

    this.diagram = function() {
        var context = this.context();
        var scale = this.totalScale();
        var radius = this.radius();
        var center = this.translateAndScale({ x : radius, y : radius });
 //        alert([radius, coordinates.x, coordinates.y]);
        context.beginPath();
        context.arc(center.x , center.y , radius * this.totalScale(), 0, Math.PI * 2);
        context.stroke();
        this.subdiagrams.diagram();
    }

    this.hitTest = function(coordinates) {
        var hit = this.subdiagrams.hitTest(coordinates);
        if (hit != undefined)
            return hit;

        var radius = this.radius();
        var center = this.translateAndScale({ x : radius, y : radius });
        var hitRadius = Math.sqrt(
            Math.pow(coordinates.x - center.x, 2) +
            Math.pow(coordinates.y - center.y, 2)
        );
        if (hitRadius <= radius * this.totalScale()) {
            return this;
        }
        return undefined;
    }
}
Function.prototype = new Diagram();

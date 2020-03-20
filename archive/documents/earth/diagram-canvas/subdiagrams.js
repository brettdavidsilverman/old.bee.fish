function Subdiagrams(parent, diagrams) {

    this.parent = parent;
    this.name = "Subdiagrams";
    this.diagrams = new Array();
    this.length = 0;
    this.previousDiagram = undefined;
    
    this.push = function(diagram) {
        diagram.parent = this;
        diagram.previousDiagram = this.previousDiagram;
        this.previousDiagram = diagram;
        diagram.oldCoordinates.push(diagram.coordinates);
        diagram.coordinates = diagramCoordinates;
        this.diagrams.push(diagram);
        this.length++;
    }

    for (var diagram in diagrams) {
        var diagram = diagrams[diagram];
        this.push(diagram);
    }

    function diagramCoordinates() {
        this.coordinates = this.oldCoordinates.pop();
        var coordinates = this.coordinates();
        this.oldCoordinates.push(this.coordinates);
        this.coordinates = diagramCoordinates;
        if (this.previousDiagram == undefined)
            return coordinates;
        var previousDiagramCoordinates = this.previousDiagram.coordinates();
        return { 
            x : coordinates.x,
            y : previousDiagramCoordinates.y + this.previousDiagram.height() + coordinates.y
        }
    }

    this.diagram = function() {

        for (var diagram in this.diagrams) {
            var diagram = this.diagrams[diagram];
            diagram.diagram();
        }
    }

    this.width = function() {
        var width = 0;
        for (var diagram in this.diagrams) {
            var diagram = this.diagrams[diagram];
            width = width < diagram.width() ? diagram.width() : width;
        }
        return width;
    }

    this.height = function() {
        var height = 0;
        for (var diagram in this.diagrams) {
            var diagram = this.diagrams[diagram];
            height += diagram.height();
        }
        return height;
    }

    this.hitTest = function(coordinates) {
        for (var diagram in this.diagrams) {
            var diagram = this.diagrams[diagram];
            var hit = diagram.hitTest(coordinates);
            if (hit != undefined) {
                return hit;
            }
        }

    }
}
Subdiagrams.prototype = new Diagram();

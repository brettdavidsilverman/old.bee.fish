function formatJSON(json, table) {

    for (var item in json) {
        var object = json[item];

        var tr = document.createElement("tr");

        if (object.unit == "url") {
            var td = document.createElement("td");
            var a = document.createElement("a");
            a.href = object.value;
            a.innerHTML = item;
            td.appendChild(a);
        }
        else {
            var tdLabel = document.createElement("td");
            tdLabel.appendChild(document.createTextNode(item));
            tr.appendChild(tdLabel);
            
            var tdValue = document.createElement("td");
            var value = object.value;
            switch (typeof(value)) {
            case "string":
                tdValue.align = "left";
                break;
            case "number":
                tdValue.align = "right";
                if (object.precision != undefined) {
                    value = value.toFixed(object.precision);
                }
                break;
            default:
                tdValue.align = "center";
            }
            tdValue.appendChild(document.createTextNode(value));
            tr.appendChild(tdValue);
    
            if (object.unit) {
                var tdUnit = document.createElement("td");
                tdUnit.appendChild(document.createTextNode(object.unit));
                tdUnit.align = "left";
                tr.appendChild(tdUnit);
            }
            else
                tdValue.colSpan = "2";
    
        }
        

        table.appendChild(tr);
    }
}

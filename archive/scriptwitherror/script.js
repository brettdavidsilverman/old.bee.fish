var script = document.createElement("script");

script.innerText = "alert('$');throw('error message');"

document.body.appendChild(script);
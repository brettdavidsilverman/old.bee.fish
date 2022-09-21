
var authentication = new Authentication();

authentication.getStatus().then(
    function(authenticated) {
        if (!authenticated) {
            document.location.href = document.location.origin + "/client/logon/"
        }
    }
)
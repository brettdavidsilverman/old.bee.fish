<?php
    require_once("errorHandling.php");
    $result = `ls /home/logs/project-web/brett -la`;
    echo "<pre>".htmlspecialchars($result)."</pre>";
?>
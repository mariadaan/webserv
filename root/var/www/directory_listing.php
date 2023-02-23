<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Directory Listing</title>
</head>
<body>
  <h1>Directory Listing</h1>
  <ul>
    <?php
    $dir = '.';
    $files = scandir($dir);
    foreach($files as $file) {
      if ($file == '.' || $file == '..') {
        continue;
      }
      if (is_dir($file)) {
        echo "<li><a href='$file/'>$file</a></li>";
      } else {
        echo "<li><a href='$file'>$file</a></li>";
      }
    }
    ?>
  </ul>
</body>
</html>

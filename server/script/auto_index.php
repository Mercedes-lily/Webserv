<?php
// Get path from command-line argument
$path = $argv[1];
echo "Here are our files in directory: $path\n";

// Start capturing output
ob_start();

// Your existing PHP script
$dh = opendir($path);
while (($file = readdir($dh)) !== false) {
    if($file != "." && $file != ".." && $file != "auto_index.php") {
        echo "<a href='$path/$file'>$file</a><br /><br />";
    }
}
closedir($dh);

// Get the captured output
$htmlOutput = ob_get_clean();

// Write the output to an HTML file
$htmlFileName = "file_list.html";
file_put_contents($htmlFileName, $htmlOutput);

echo "HTML file '$htmlFileName' has been created.\n";
?>
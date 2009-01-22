<?php
header("Content-Type: audio/mpegurl");
header("Content-Disposition: attachment; filename=playlist.m3u");

require_once "ktvFunctions.inc";

$user = isset($HTTP_GET_VARS['user']) ? $HTTP_GET_VARS['user'] : "148";
$pass = isset($HTTP_GET_VARS['pass']) ? $HTTP_GET_VARS['pass'] : "841";

$ktvFunctions = new KtvFunctions($user, $pass);
$content = $ktvFunctions->getChannelsList();

# compact all to a single line
$content = preg_replace('|\n|', '', $content);

# no spaces between tags
$content = preg_replace('|>\s+|', '>', $content);
$content = preg_replace('|\s+<|', '<', $content);

# parse out channels
preg_match_all('/<channel\s.*?id="(.*?)".*?title="(.*?)".*?idx="(.*?)".*?>/', $content, $matches);
$ids   = $matches[1];
$names = $matches[2];
$nums  = $matches[3];
$path  = getFullUrlPath();

# generate header
print '#EXTM3U' . "\n";

# generate channel entries
for ($i = 0; $i < count($ids); $i++) {
    print '#EXTINF:0,';
    print $nums[$i] . ". " . $names[$i] . " (" . $ids[$i] . ")\n";
    print $path . "stream.php?id=$ids[$i]&user=$user&pass=$pass\n";
}


# returns full URL of current script
function getFullUrlPath() { 
    $protocol = strtolower($_SERVER['SERVER_PROTOCOL']);
    $protocol = substr($protocol, 0, strpos($protocol, '/'));
    $protocol .= "on" === $_SERVER['HTTPS'] ? "s" : "";
    $port = "80" === $_SERVER['SERVER_PORT'] ? "" : (":" . $_SERVER['SERVER_PORT']);
    $url = $protocol . "://" . $_SERVER['SERVER_NAME'] . "$port";
    $path = $_SERVER['REQUEST_URI'];
    $path = substr($path, 0, strrpos($path, '/') + 1);
    return $url . $path;
}

?>

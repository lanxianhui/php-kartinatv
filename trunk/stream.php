<?php
require_once "ktvFunctions.inc";

$user = isset($HTTP_GET_VARS['user']) ? $HTTP_GET_VARS['user'] : "148";
$pass = isset($HTTP_GET_VARS['pass']) ? $HTTP_GET_VARS['pass'] : "841";
$id   = isset($HTTP_GET_VARS['id'])   ? $HTTP_GET_VARS['id']   : 7;

$ktvFunctions = new KtvFunctions($user, $pass);

$content = $ktvFunctions->getStreamUrl($id);
$url = preg_replace('/.*url="http(\/ts|)([^ "]*).*/', 'http$2', $content); 

header("Location: $url");

?>

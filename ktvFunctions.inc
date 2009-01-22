<?php
#############################################################################
# Library with all functions needed to connect kartina.tv.                  #
# These functions allow to log on to kartina.tv, get information about      #
# available channels and to get the channels URLs themselves.               #
#                                                                           #
# Author: consros 2008                                                      #
#############################################################################

# set this flag to TRUE if you want the adult channels to be available
define("ALLOW_EROTIC", TRUE);


class KtvFunctions {
    var $cookie;
    var $traces;

    function KtvFunctions($username, $password, $cookie = "", $traces = false) {
        $this->username = $username;
        $this->password = $password;
        $this->cookie   = $cookie;
        $this->traces   = $traces;
    }

    function trace($msg) {
        if ($this->traces) {
            print "DEBUG: " . $this->username . ": $msg\n";
        }
    }

    function authorize() {
        $this->trace("Authorization started");
        $request  = "act=login";
        $request .= "&code_login=" . $this->username;
        $request .= "&code_pass="  . $this->password;
        $this->cookie = getCookies("http://iptv.kartina.tv", $request);
        strpos($this->cookie, "SSID") or 
            die ($this->username . ": Authorization of user failed!");
        strpos($this->cookie, "=deleted") and 
            die ($this->username . ": Wrong authorization request");
        $this->trace("Authorization returned: $this->cookie");
    }

    function isAuthorized($reply) {
        $ok = false == strpos($reply, "code_login") && 
            false == strpos($reply, "code_pass");
        if (! $ok) {
            $this->trace("Authorization missed or lost");
        }
        return $ok;
    }

    function getData($url, $name) {
        $this->trace("Getting $name");
        $reply = getPageContent($url, $this->cookie);
        if (! $this->isAuthorized($reply)) {
            $this->authorize(); 
            $this->trace("Second try to get $name");
            $reply = getPageContent($url, $this->cookie);
            $this->isAuthorized($reply) or 
                die ("Failed to get $name:\n$reply");
        }
        return $reply;
    }

    function getChannelsList() {
        $url  = "http://iptv.kartina.tv/";
        $url .= "?m=channels&act=get_list_xml";
        return $this->getData($url, "channels list");
    }

    function getStreamUrl($id) {
        $url  = "http://iptv.kartina.tv/";
        $url .= "?m=channels&act=get_stream_url&cid=$id";
        if (ALLOW_EROTIC) {
            $url .= "&protect_code=" . $this->password;
        }
        return $this->getData($url, "URL of stream $id");
    }

    function getEpg($id, $date = null) {
        $date = isset($date) ? date('dmy', $date) : date('dmy');
        $url  = "http://iptv.kartina.tv/";
        $url .= "?m=epg&act=show_day_xml&day=$date&cid=$id";
        return $this->getData($url, "EPG for channel $id");
    }
}



function getPageContent($url, $cookie) {
    $opts = array(
        'http' => array(
            'method'=> 'GET',
            'header'=> "User-Agent: Mozilla/5.0\r\n$cookie")
        );
    $context = stream_context_create($opts);
    $f = fopen($url, "r", false, $context);
    if (! $f) {
        die ("Cannot open $host");
    }
    while($data = fread($f, 1024)) {
        $content .= $data;
    }
    return $content;
}

function getCookies($url, $request, $headerExt = "") {
    $parsedUrl = parse_url($url);
    $host = $parsedUrl['host'];
    $path = $parsedUrl['path'];
    if ("" == $path) {
        $path = "/";
    }

    $header  = "POST $path HTTP/1.1\r\n";
    $header .= "Host: $host\r\n";
    $header .= "User-Agent: Mozilla/5.0\r\n";
    $header .= "Content-Type: application/x-www-form-urlencoded\r\n";
    $header .= "Content-Length: " . strlen($request) . "\r\n";
    $header .= $headerExt;
    $header .= "\r\n";

    $fp = fsockopen($host, 80, $errno, $errstr, 30); 
    if (! $fp) {
        die ("Cannot open $host");
    }

    fputs($fp, $header . $request);
    while (! feof($fp)) {
        $reply .= fgets($fp, 1024);
    }
    fclose($fp);

    preg_match_all("/Set-Cookie: .*?;/", $reply, $matches);
    foreach ($matches[0] as $cookie) {
        $cookies .= substr($cookie, 4, strlen($cookie) - 5) . "\r\n";
    }

    return $cookies;
}

?>

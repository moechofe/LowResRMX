<?php

header_remove('X-Powered-By');

error_reporting(E_ALL);

require_once __DIR__.'/private.php';

const WEBSITE_URL='http://localhost:8080';

const HEADER_TOKEN='X-Application-Token';
const HEADER_FILE_TYPE='X-Application-Type';
const HEADER_SESSION='X-Application-Session';

const UPLOAD_TOKEN_TTL=60*60; // 1 hour
const LOGIN_GOOGLE_TTL=60*5; // 5 minutes

const SYLLABLE_LIST=["ing","er","a","ly","ed","i","es","re","tion","in","e","con","y","ter","ex","al","de","com","o","di","en","an","ty","ry","u","ti","ri","be","per","to","pro","ac","ad","ar","ers","ment","or","tions","ble","der","ma","na","si","un","at","dis","ca","cal","man","ap","po","sion","vi","el","est","la","lar","pa","ture","for","is","mer","pe","ra","so","ta","as","col","fi","ful","ger","low","ni","par","son","tle","day","ny","pen","pre","tive","car","ci","mo","on","ous","pi","se","ten","tor","ver","ber","can","dy","et","it","mu","no","ple","cu","fac","fer","gen","ic","land","light","ob","of","pos","tain","den","ings","mag","ments","set","some","sub","sur","ters","tu","af","au","cy","fa","im","li","lo","men","min","mon","op","out","rec","ro","sen","side","tal","tic","ties","ward","age","ba","but","cit","cle","co","cov","da","dif","ence","ern","eve","hap","ies","ket","lec","main","mar","mis","my","nal","ness","ning","n't","nu","oc","pres","sup","te","ted","tem","tin","tri","tro","up","va","ven","vis","am","bor","by","cat","cent","ev","gan","gle","head","high","il","lu","me","nore","part","por","read","rep","su","tend","ther","ton","try","um","uer","way","ate","bet","bles","bod","cap","cial","cir","cor","coun","cus","dan","dle","ef","end","ent","ered","fin","form","go","har","ish","lands","let","long","mat","meas","mem","mul","ner","play","ples","ply","port","press","sat","sec","ser","south","sun","the","ting","tra","tures","val","var","vid","wil","win","won","work","act","ag","air","als","bat","bi","cate","cen","char","come","cul","ders","east","fect","fish","fix","gi","grand","great","heav","ho","hunt","ion","its","jo","lat","lead","lect","lent","less","lin","mal","mi","mil","moth","near","nel","net","new","one","point","prac","ral","rect","ried","round","row","sa","sand","self","sent","ship","sim","sions","sis","sons","stand","sug","tel","tom","tors","tract","tray","us","vel","west","where","write"];

const GOOGLE_CLIENT_ID='204783433847-6fi9h1vl0v1ab682i3r0d1fqffmeo5m4.apps.googleusercontent.com';
const GOOGLE_CLIENT_SECRET='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';

const MAX_AUTHOR_NAME=256;

enum ScoreActivity
{
	case Publishing;
	case Updating;
	case Upvoting;
}

function computeScoreIncrease(Client $client,string $program_id,ScoreActivity $activity):void
{
	switch($activity)
	{
	case ScoreActivity::Publishing:
		// read the activity of the website, number of upvote per day
		// and set the same value to the publish score

		break;
	}
}

require_once __DIR__.'/redis.php';

function badRequest(string $reason):void
{
	header("HTTP/1.1 400 Bad Request",true,400);
	trigger_error($reason);
	exit;
}

function forbidden(string $reason):void
{
	header("HTTP/1.1 403 Forbitten",true,403);
	trigger_error($reason);
	exit;
}

function internalServerError(string $reason):void
{
	header("HTTP/1.1 500 Internal Server Error",true,500);
	trigger_error($reason);
	exit;
}

function redis():Client
{
	static $client=null;
	if($client===null) $client=new Client('tcp://127.0.0.1:6379');
	if(!$client) internalServerError("Fail to reach db");
	return $client;
}

function revokeSession(string $session_id):void
{
	$user_id=redis()->hget("s:$session_id","uid");
	redis()->del("s:$session_id");
	if($user_id) redis()->lrem("u:$user_id:s",0,$session_id);
}

function validateSessionAndGetUserId():string
{
	$session_id=@hex2bin(getallheaders()[HEADER_SESSION]);
	if(!$session_id) return "";

	$status=redis()->hget("s:$session_id","status");

	switch($status)
	{
	case "revoked":
		revokeSession($session_id);
		return "";

	case "allowed": default:
		$user_id=redis()->hget("s:$session_id","uid");
		if(!$user_id) $user_id="";
		return $user_id;
	}
}

$request=$_SERVER['REQUEST_URI'];
$url=parse_url($request);
$info=pathinfo($url['path']);
$query=$_SERVER['QUERY_STRING'];

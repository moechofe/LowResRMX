<?php

require_once __DIR__.'/common.php';

if(strlen($info['filename'])>16&&strlen($info['filename'])<512&&in_array($info['extension'],['prg','img']))
{
	$program_id=pathinfo($request,PATHINFO_FILENAME);

	$type=pathinfo($request,PATHINFO_EXTENSION);
	if($type!=='prg'&&$type!=='img') badRequest("Fail to read type",__FILE__,__LINE__);

	// TODO: check publish status

	$source=redis()->hget("p:$program_id",$type);
	$source=zstd_uncompress($source);

	echo $source;
	exit;
}

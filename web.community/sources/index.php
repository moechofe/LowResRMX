<?php

require_once __DIR__.'/common.php';
require_once __DIR__.'/token.php';

// order is important
require_once __DIR__.'/google2.php';
require_once __DIR__.'/google1.php';

// order is important
require_once __DIR__.'/discord2.php';
require_once __DIR__.'/discord1.php';

require_once __DIR__.'/twitter1.php';

require_once __DIR__.'/upload.php';
require_once __DIR__.'/download.php';

require_once __DIR__.'/share.php';
require_once __DIR__.'/program_list.php';
require_once __DIR__.'/publish.php';
require_once __DIR__.'/delete.php';






require_once __DIR__.'/signed.php';
// require_once __DIR__.'/published.php';

// require_once __DIR__.'/player.php';

// require_once __DIR__.'/trends.php';
require_once __DIR__.'/my_programs.php';
require_once __DIR__.'/program.php';

require_once __DIR__.'/post.php';
require_once __DIR__.'/entry.php';







require_once __DIR__.'/static.php';

if($url['path']==='/')
{
	header("Status: 301 Moved Permanently",true,301);
	header("Location: community.html");
	exit();
}

header("Status: 404 Not Found",true,404);
exit();

<!doctype html public "-//W3C//DTD HTML 4.01 Transitional//EN">
<!--
               /* pic.php */

/* This is part of the AstroCam Webinterface PHP Code written by
 * Steffen Wendzel, https://www.wendzel.de
 *
 * Copyright 2001-2008 by Steffen Wendzel
 */
-->


<?   /* init */
	$picfile = $HTTP_GET_VARS['picfile'];
	$refreshtime = $HTTP_GET_VARS['refreshtime'];
	
	/* check refreshtime value */
	if(!ereg("^([1-9]+[0-9]*)$", $refreshtime, $regs)) {
		die('refreshtime value not allowed (must be at least 1)!');
	}
	/* check picfile url */
	if($picfile == "")
		die('no picfile value set!');
	if(ereg("[<>\'\&\"]+", $picfile, $regs)) {
		die('picfile value not allowed!');
	}
	
?>

<html>
<head>
	<meta http-equiv="pragma" content="no-cache">
	<meta http-equiv="refresh" content="<? echo $refreshtime; ?>;pic.php<? printf("?picfile=%s&refreshtime=%s", $picfile, $refreshtime); ?>">
	<link rel="stylesheet" type="text/css" href="design.css" title="primary">
</head>
<body>
<table>
<tr>
	<td class="menu">
		<img src="<? echo $picfile; ?>" alt="current image">
	</td>
</tr>
</body>
</html>


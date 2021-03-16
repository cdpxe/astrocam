<!doctype html public "-//W3C//DTD HTML 4.01 Transitional//EN">

<!--
               /* astrocam.php */

/* This is the AstroCam Webinterface PHP Code written by
 * Steffen Wendzel, https://www.wendzel.de
 *
 * Copyright 2001-2009 by Steffen Wendzel
 */
-->

<?php
	/* init the vars */
	$version="2.7.6";
	$a = $HTTP_GET_VARS['a'];

	$file=fopen("/etc/astrocam.conf", "r");
	if($file==FALSE) {
		printf("cannot open astrocam.conf!");
		exit(1);
	}
	while ($data = fscanf($file, "%s\t%s\n"))
	{
		list ($var, $value) = $data;		
		if(strcmp($var, "version")==0){
			if(strcmp($value, $version)!=0){
				printf("wrong conf-version!");
				exit(1);
			}
		/*}else if(strcmp($var, "maxdreh")==0){
			$maxdreh=$value;
		}else if(strcmp($var, "mindreh")==0){
			$mindreh=$value;
		}else if(strcmp($var, "maxvdreh")==0){
			$maxvdreh=$value;
		}else if(strcmp($var, "minvdreh")==0){
			$minvdreh=$value;*/
		}else if(strcmp($var, "picfile")==0){
			$picfile=$value;
		}else if(strcmp($var, "refreshtime")==0){
			$refreshtime=$value;
		}else if(strcmp($var, "design")==0){
			$design=$value;
		}else if(strcmp($var, "ipckey")==0){
			$ipckey=$value;
		}else if(strcmp($var, "imagesize-y")==0){
			$height=$value;
		}else if(strcmp($var, "imagesize-x")==0){
			$width=$value;
		}else if(strcmp($var, "contenturl")==0){
			$contenturl=$value;
		}
	}
	fclose($file);

	if(/*$maxdreh == "" || $mindreh == "" ||*/ $picfile == "" || $refreshtime == "" ||
	   $ipckey == "" || $design == "" || $version == "" || $height == "" ||
	   $width == "" || $contenturl == "") {
		printf("At least 1 needed variable is not present in astrocam.conf!");
		exit(1);
	}

	if(isset($a)) {
		if (!ereg('^[lLrRdu]$', $a)) {
			printf("unexpected character submitted. exiting.\n");
			exit(1);
		}
		if(isset($debug))
			printf("%s <b>|</b> ", $a);

		$mq=msg_get_queue($ipckey, 0);
		if(!msg_send($mq, 1, $a, true, true, $error)) {
			printf("Msg not sent (error=$error)\n");
			printf("Are you sure that the astrocam daemon is running?\n");
			exit(1);
		} else {
			if(isset($debug)) {
				printf("message successfully send.");
			}
		}
	}
?>

<!---
 ------------------- ------------------- ------------------- -------------------
	 		H T M L    S T U F F    S T A R T
 ------------------- ------------------- ------------------- -------------------
-->

<html>
<head>
	<meta http-equiv="Content-Type" content="text/html;charset=iso-8859-1">
	<link rel="stylesheet" type="text/css" href="<? echo $contenturl.'/'.$design; ?>.css" title="prim">
	<title>AstroCam Webinterface</title>
</head>
<body>
<table align="center">
<tr>
	<td colspan="2">
		<img src="<?php echo $contenturl; ?>/spacer.gif" height="20px" width="1px" alt=".">
	</td>
</tr>
<tr>
	<td align="center" valign="top" class="menu">
		<img src="<?php echo $contenturl; ?>/spacer.gif" alt="." width="80px" height="1px"><br>
		<b>[<a href="?a=l">&lt;</a>]</b>
		<b>[<a href="?a=r">&gt;</a>]</b>
		<br>
		<b>[<a href="?a=L">&lt;&lt;</a>]</b>
		<b>[<a href="?a=R">&gt;&gt;</a>]</b>
		<p>
		<b>[<a href="?a=u">up</a>]</b><br>
		<b>[<a href="?a=d">down</a>]</b>		
	</td>
	<td align="center" valign="center" class="pic"
	     width="<? echo $width; ?>" height="<? echo $height; ?>">
		<iframe border="0" width="<? echo $width; ?>" height="<? echo $height; ?>"
		   src="pic.php?picfile=<? echo $picfile; ?>&refreshtime=<? echo $refreshtime; ?>"
		   scrolling="no" marginwidth="0" marginheight="0">
		Your browser doesn't support iframes!
		</iframe>
	</td>
</tr>
<tr>
	<td align="right" colspan="2" class="menu">
		<div style="font-size:9pt">
			<b>astrocam webinterface</b>
			[<a href="https://www.wendzel.de">info</a>]
		</div>
	</td>
</tr>
</table>
</body>
</html>

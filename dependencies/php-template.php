<?php 
/***University of South Carolina - Music Library***
 * Copyright (c) 2016 University of South Carolina
 * Author: Adrian Quiroga
 */
?>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">

<link type="text/css" href="resources/skin/blue.monday/css/jplayer.blue.monday.css" rel="stylesheet" />
<link type="text/css" href="style.css" rel="stylesheet" />

<script type="text/javascript" src="resources/jquery-1.11.3.min.js"></script>
<script type="text/javascript" src="resources/jquery.jplayer.min.js"></script>
<script type="text/javascript" src="resources/jplayer.playlist.min.js"></script>

<?php[category-data]?>

<script type="text/javascript">
$(document).ready(function(){
		<?php
		$counter = 1;
		foreach ($playlists as $playlist):
		if ($playlist["type"]=="playlist"):
		?>
      $("#jquery_jplayer_<?php print $counter?>").jPlayer({
        ready: function(){},
        cssSelectorAncestor: "#jp_container_<?php print $counter?>",
        swfPath: "/js",
        supplied: "mp3",
        useStateClassSkin: true,
        autoBlur: false,
        smoothPlayBar: true,
        keyEnabled: true,
        remainingDuration: true,
        toggleDuration: true
      });
	  
	  <?php
	  $counter++;
	  endif;
	  endforeach;?>
    });

function loadFunction(){
	//alert ('test');
	<?php
	$counter = 1;
	foreach ($playlists as $playlist):
	if ($playlist["type"]=="playlist"):
	?>
	var myPlaylist = new jPlayerPlaylist({
		jPlayer: "#jquery_jplayer_<?php print $counter?>",
		cssSelectorAncestor: "#jp_container_<?php print $counter?>"
		},<?php 
		print json_encode($playlist["tracks"]);
		?>,
		{
			playlistOptions: {
				enableRemoveControls: false
			},
			swfPath: "/js",
			supplied: "mp3",
			smoothPlayBar: true,
			keyEnabled: true,
			audioFullScreen: true // Allows the audio poster to go full screen via keyboard
		}
	);
	<?php
	  $counter++;
	  endif;
	  endforeach;?>
}
</script>


<title>Music Library - USC</title>
</head>

<body onLoad="loadFunction();">
<div class="container">
<!--
<h1><?php //print $categoryTitle;?></h1>
<p><?php //print $categoryDescription;?></p>-->
<?php 
$counter = 1;
foreach ($playlists as $playlist) : 
if ($playlist["type"] == "playlist") : ?>
<!--<hr>-->
<p><strong><big><?php print $playlist["title"]?></big></strong>
<p><?php print $playlist["description"]?></p>
<div class="jplayerContainer">
<div id="jquery_jplayer_<?php print $counter?>" class="jp-jplayer"></div>
<div id="jp_container_<?php print $counter?>" class="jp-audio" role="application" aria-label="media player">
  <div class="jp-type-single">
    <div class="jp-gui jp-interface">
      <div class="jp-volume-controls">
        <button class="jp-mute" role="button" tabindex="0">mute</button>
        <button class="jp-volume-max" role="button" tabindex="0">max volume</button>
        <div class="jp-volume-bar">
          <div class="jp-volume-bar-value"></div>
        </div>
      </div>
      <div class="jp-controls-holder">
        <div class="jp-controls">
          <button class="jp-play" role="button" tabindex="0">play</button>
          <button class="jp-stop" role="button" tabindex="0">stop</button>
        </div>
        <div class="jp-progress">
          <div class="jp-seek-bar">
            <div class="jp-play-bar"></div>
          </div>
        </div>
        <div class="jp-current-time" role="timer" aria-label="time">&nbsp;</div>
        <div class="jp-duration" role="timer" aria-label="duration">&nbsp;</div>
        <div class="jp-toggles">
          <button class="jp-repeat" role="button" tabindex="0">repeat</button>
        </div>
      </div>
    </div>
    <div class="jp-details">
      <div class="jp-title" aria-label="title">&nbsp;</div>
    </div>
    <div class="jp-playlist">
    	<ul>
        	<li></li> <!-- Empty <li> so your HTML conforms with the W3C spec -->
        </ul>
    </div>
    <div class="jp-no-solution">
      <span>Update Required</span>
      To play the media you will need to either update your browser to a recent version or update your <a href="http://get.adobe.com/flashplayer/" target="_blank">Flash plugin</a>.
    </div>
  </div>
</div>
</div>
<?php 
$counter++;
//endif; //if ($playlist["type"] == "playlists")
elseif ($playlist["type"] == "link"): 
?>
<!--<hr>-->
<div>
	<p><strong><big><?php print $playlist["title"]?></big></strong>
    <?php print $playlist["description"]?>--<a href="<?php print $proxyPrefix.$playlist["permalink"]?>" target="_blank">Click here to access</a></p>
</div>

<?php 
else:
?>
<div>
	<?php if ($playlist["displayDividerTitle"] == "1"):
	?>
	<h2><?php print $playlist["title"]?></h2>
    <?php 
	endif;
	if ($playlist["displayDividerHR"] == "1"):?>
	<hr>
	<?php endif;?>
</div>
<?php 
endif;
endforeach; ?>
<br>
<br>
</div> <!-- container-->
</body>
</html>
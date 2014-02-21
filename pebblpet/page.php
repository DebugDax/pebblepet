<?php
$tut = (isset($_GET['tut'])) ? $_GET['tut'] : 0;

$sVer = 1;
$cVer = (isset($_GET['cver'])) ? $_GET['cver'] : '1.0';

$petsync = (isset($_GET['petsync']) && $_GET['petsync'] <> 'undefined') ? $_GET['petsync'] : 1;

$age = (isset($_GET['age'])) ? $_GET['age'] : '';
$happiness = (isset($_GET['happiness'])) ? $_GET['happiness'] : '';
$health = (isset($_GET['health'])) ? $_GET['health'] : '';
$level = (isset($_GET['level'])) ? $_GET['level'] : '';
$timeleft = (isset($_GET['timeleft'])) ? $_GET['timeleft'] : '';
$timealive = (isset($_GET['timealive'])) ? $_GET['timealive'] : '';

$days = round($timealive/24);
$age = ($days < 0) ? $hours . " hours" : $days . " days";

if ($timeleft >= 61)
{
 $timeleft = floor($timeleft / 60) . "h";
} else {
 $timeleft = $timeleft . "m";
}

?>

<!DOCTYPE html>
<html>
  <head>
    <title>PebblePet</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="http://code.jquery.com/mobile/1.3.2/jquery.mobile-1.3.2.min.css" />
    <script src="http://code.jquery.com/jquery-1.9.1.min.js"></script>
    <script src="http://code.jquery.com/mobile/1.3.2/jquery.mobile-1.3.2.min.js"></script>
  </head>
<body>
 <body>
    <div data-role="page" id="main">
      <div data-role="header" class="jqm-header">
        <h1>PebblePet</h1>
      </div>

      <div data-role="content">

	  <?php if ($tut <= 0) { ?>
	  
        <div class="ui-body ui-body-b">
        <div data-role="fieldcontain">
          <p>Welcome to PebblePet!</p>
		  
		  <p>
		  You are about to adopt your own virtual pet that will grow with you.
		  Just an egg at the start, there's no telling what he may become at the end!
		  </p>
		  
		  <p>
		  Your pet takes a certain amount of time to evolve, during which he must be cared for. 
		  Ignoring your pet will result in happiness and health being decreased which in turn affects what your pet grows up to be.<br/><br/>
		  Not opening the app and visiting your pet enough, not shaking your wrist to get rid of any mess that may be laying around, and not giving affection to your pet will all affect his health and happiness.<br/><br/>
		  There are six evolutions to your pet and each evolution has three possible paths.
		  <ul>
		  <li>Egg</li>
		  <li>Baby</li>
		  <li>Child</li>
		  <li>Teen</li>
		  <li>Adult</li>
		  <li>Elderly</li>
		  </ul>
		  At any point you may release your pet into the wild and adopt a new one. Caring, isn't it?<br/><br/>
		  That's it for the basic information.<br/>
		  One additional fact, by using the options page in the pebble application on your phone you can claim codes to unlock new pets! The codes will be given out randomly by @Sirhiro.
		  </p>
          <div ><button type="submit" data-theme="a" id="a-submit">Get Started</button></div>
        </div>
	  
	  
	  <?php } else { 
	  if ($sVer > $cVer) {
        echo '<div style="text-align: center; vertical-align: middle;" class="ui-body ui-body-b">
		  <label>New version available!</label><br/>
		  <label>Installed: ' . $cVer . '</label><br/>
		  <label>Newest: ' . $sVer . '</label><br/>
        </div>';
	}
	  ?>
        <div data-role="fieldcontain">
		  <label style="text-decoration:underline">Current Stats</label><br/>
          <label for="level">Level: <label id='level'><?php echo $level; ?></label></label><br/>
          <label for="age">Age: <label id='age'><?php echo $age; ?></label></label> old<br/>
          <label for="happiness">Happiness: <label id='happy'><?php echo $happiness; ?></label></label>%<br/>
          <label for="health">Health: <label id='hp'><?php echo $health; ?></label></label>%<br/>
          <label for="timeleft">Grow in: <label id='time'><?php echo $timeleft; ?></label></label><br/>
        </div>
		
		<div style="text-align: center; vertical-align: middle;" class="ui-body ui-body-b">
          <fieldset class="ui-grid-a">
              <label>PetSync<label><br/>
			  <?php
			  if ($petsync >= 1) {
					echo '<select onchange="javascript:document.location=\'pebblejs://close#notrack\';" "name="special-feature" id="special-feature" data-role="slider">';
					echo '<option value="off">Off</option><option selected="selected" value="on">On</option></select>';
			  } else {
					echo '<select onchange="javascript:document.location=\'pebblejs://close#track\';" "name="special-feature" id="special-feature" data-role="slider">';
					echo '<option selected="selected" value="off">Off</option><option value="on">On</option></select>';
			  }
			  ?>
            </fieldset>
        </div>
		
		<div style="text-align: center; vertical-align: middle;" class="ui-body ui-body-b">
          <fieldset class="ui-grid-a">
              <label>Reset Data<label><br/>
              <div><button type="submit" data-theme="a" id="b-submit-d">Start Over</button></div>
            </fieldset>
        </div>
		<?php } ?>
        </div>
      </div>
	<script>
		
        $().ready(function() {
		
        $("#b-cancel").click(function() { document.location = "pebblejs://close"; });
        $("#a-submit").click(function() { document.location = "pebblejs://close#tut"; });
        $("#b-submit-d").click(function() { alert('Exit PebblePet on your Pebble to finish reset...'); document.location = "pebblejs://close#reset"; });

        $("#b-submit").click(function() {
			var timey = $('label#time').text().substring(0, $('label#time').text().length - 1);
		  document.location = "http://domain.com/page.php?code=" + $('textarea#code').val() + "&level=" + $('label#level').text() + "&age=" + $('label#age').text() + "&happiness=" + $('label#happy').text() + "&health=" + $('label#hp').text() + "&timeleft=" + timey + "&tut=1";
        });

      });

</script>
</body>
</html>
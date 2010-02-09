--TEST--
Check basic functionality of check_plain().
--SKIPIF--
<?php if (!extension_loaded("drupal_extension")) print "skip"; ?>
--FILE--
<?php
  echo check_plain('test & <toto>');
?>
--EXPECT--
test &amp; &lt;toto&gt;
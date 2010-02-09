--TEST--
Test basic drupal_static() functionnality
--SKIPIF--
<?php if (!extension_loaded("drupal_extension")) print "skip"; ?>
--FILE--
<?php 
$ret = &drupal_static("test", array());
if (is_array($ret) && empty($ret)) {
  echo "Initial value is preserved.\n";
}

$ret['test_value'] = 'toto';
$ret2 = &drupal_static("test", array());
if (isset($ret2['test_value'] && $ret2['test_value'] == 'toto')) {
  echo "Stored value is preserved.\n";
}
?>
--EXPECT--
Initial value is preserved.
Stored value is preserved.

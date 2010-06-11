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
if (isset($ret2['test_value']) && $ret2['test_value'] == 'toto') {
  echo "Stored value is preserved.\n";
}

$ret = &drupal_static('test', NULL, TRUE);
if (is_array($ret) && empty($ret)) {
  echo "Reset key successful.\n";
}

$ret = &drupal_static('test2', NULL);
if (is_array($ret) && empty($ret)) {
  echo "Initial value with NULL as default is preserved.\n";
}

$failed = FALSE;
for($i = 0; $i < 50; $i++) {
  if (loop_stability_test('loop_stability_test_key') == FALSE || loop_stability_test(get_rand_str()) == FALSE) {
    $failed = TRUE;
    break;
  }
}

if (!$failed) {
  echo "Passed loop stability test.\n";
}

function loop_stability_test($key) {
  $ret = &drupal_static($key, array());
  if (!is_array($ret) || !empty($ret)) {
    return FALSE; 
  }

  $ret["$key $key"] = $key;
  $ret2 = &drupal_static($key);
  if (!isset($ret2["$key $key"]) || $ret2["$key $key"] != $key) {
    return FALSE;
  }

  $ret = &drupal_static($key, NULL, TRUE);
  if (!is_array($ret) || !empty($ret)) {
    return FALSE;
  }

  $ret = &drupal_static($key, NULL);
  if (!is_array($ret) || !empty($ret)) {
    return FALSE;
  }

  return TRUE;
}

function get_rand_str() {
  $str = '';
  for ($i=0; $i<10; $i++) {
    $str .= chr(rand(65,90));
  }
  return $str;
}

?>
--EXPECT--
Initial value is preserved.
Stored value is preserved.
Reset key successful.
Initial value with NULL as default is preserved.
Passed loop stability test.

--TEST--
Test basic drupal_static() functionnality
--SKIPIF--
<?php if (!extension_loaded("drupal_extension")) print "skip"; ?>
--FILE--
<?php 
$ret = &drupal_static("test", array());
if (is_array($ret) && empty($ret)) {
  echo "Initial array value is preserved.\n";
}

$ret['test_value'] = 'toto';
$ret2 = &drupal_static("test", array());
if (isset($ret2['test_value']) && $ret2['test_value'] == 'toto') {
  echo "Stored array value is preserved.\n";
}

$ret = &drupal_static('test', NULL, TRUE);
if (is_array($ret) && empty($ret)) {
  echo "Reset key with array value successful.\n";
}

$ret = &drupal_static('test_scalar', 'default');
if (is_scalar($ret) && $ret == 'default') {
  echo "Initial scalar value is preserved.\n";
}

$ret = 'default_changed';
$ret2 = &drupal_static('test_scalar');
if (is_scalar($ret2) && $ret2 == 'default_changed') {
  echo "Stored scalar value is preserved.\n";
}

$ret = &drupal_static('test_scalar', NULL, TRUE);
if (is_scalar($ret) && $ret == 'default') {
  echo "Reset key with scalar value successful.\n";
}

$ret = &drupal_static('test2', NULL);
$ret['foo_key'] = 'foo_value';
$ret = &drupal_static('test2');
if (is_array($ret) && isset($ret['foo_key']) && $ret['foo_key'] == 'foo_value') {
  echo "Passed test with NULL as default.\n";
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
Initial array value is preserved.
Stored array value is preserved.
Reset key with array value successful.
Initial scalar value is preserved.
Stored scalar value is preserved.
Reset key with scalar value successful.
Passed test with NULL as default.
Passed loop stability test.

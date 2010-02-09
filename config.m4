PHP_ARG_ENABLE(drupal_extension, whether to enable drupal_extension support,
[  --enable-drupal_extension           Enable drupal_extension support])

if test "$PHP_DRUPAL_EXTENSION" != "no"; then
  PHP_NEW_EXTENSION(drupal_extension, drupal_extension.c, $ext_shared)
fi

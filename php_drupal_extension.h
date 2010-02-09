
#ifndef PHP_DRUPAL_EXTENSION_H
#define PHP_DRUPAL_EXTENSION_H

extern zend_module_entry drupal_extension_module_entry;
#define phpext_drupal_extension_ptr &drupal_extension_module_entry

#ifdef PHP_WIN32
#define PHP_DRUPAL_EXTENSION_API __declspec(dllexport)
#else
#define PHP_DRUPAL_EXTENSION_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(drupal_extension);
PHP_MSHUTDOWN_FUNCTION(drupal_extension);
PHP_RINIT_FUNCTION(drupal_extension);
PHP_RSHUTDOWN_FUNCTION(drupal_extension);
PHP_MINFO_FUNCTION(drupal_extension);

PHP_FUNCTION(check_plain);
PHP_FUNCTION(drupal_static);

ZEND_BEGIN_MODULE_GLOBALS(drupal_extension)
	zval *drupal_static_zdata;
  zval *drupal_static_zdefault;
ZEND_END_MODULE_GLOBALS(drupal_extension)

#ifdef ZTS
#define DRUPAL_EXTENSION_G(v) TSRMG(drupal_extension_globals_id, zend_drupal_extension_globals *, v)
#else
#define DRUPAL_EXTENSION_G(v) (drupal_extension_globals.v)
#endif

#endif	/* PHP_DRUPAL_EXTENSION_H */


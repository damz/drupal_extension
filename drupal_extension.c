
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/html.h"
#include "php_drupal_extension.h"

ZEND_DECLARE_MODULE_GLOBALS(drupal_extension)

/* True global resources - no need for thread safety here */
static int le_drupal_extension;

/* {{{ arginfo_drupal_static */
ZEND_BEGIN_ARG_INFO_EX(arginfo_check_plain, 0, 0, 1)
	ZEND_ARG_INFO(0, text)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ arginfo_drupal_static */
ZEND_BEGIN_ARG_INFO_EX(arginfo_drupal_static, 0, 1, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, default)
	ZEND_ARG_INFO(0, reset)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ drupal_extension_functions[]
 *
 * Every user visible function must have an entry in drupal_extension_functions[].
 */
zend_function_entry drupal_extension_functions[] = {
	PHP_FE(check_plain, arginfo_check_plain)
	PHP_FE(drupal_static_experimental,	arginfo_drupal_static)
	{NULL, NULL, NULL}	/* Must be the last line in drupal_extension_functions[] */
};
/* }}} */

/* {{{ drupal_extension_module_entry
 */
zend_module_entry drupal_extension_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"drupal_extension",
	drupal_extension_functions,
	PHP_MINIT(drupal_extension),
	PHP_MSHUTDOWN(drupal_extension),
	PHP_RINIT(drupal_extension),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(drupal_extension),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(drupal_extension),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_DRUPAL_EXTENSION
ZEND_GET_MODULE(drupal_extension)
#endif

/* {{{ php_drupal_extension_init_globals
 */
static void php_drupal_extension_init_globals(zend_drupal_extension_globals *drupal_extension_globals)
{
	MAKE_STD_ZVAL(drupal_extension_globals->drupal_static_zdata);
	array_init(drupal_extension_globals->drupal_static_zdata);
	MAKE_STD_ZVAL(drupal_extension_globals->drupal_static_zdefault);
	array_init(drupal_extension_globals->drupal_static_zdefault);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(drupal_extension)
{
	ZEND_INIT_MODULE_GLOBALS(drupal_extension, php_drupal_extension_init_globals, NULL);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(drupal_extension)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(drupal_extension)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(drupal_extension)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(drupal_extension)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "drupal_extension support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ proto string check_plain(string text)
   Experimental implementation of Drupal check_plain(). */
PHP_FUNCTION(check_plain)
{
	char *str;
	int str_len;
	int len;
	char *replaced;
	zend_bool double_encode = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}
	replaced = php_escape_html_entities_ex(str, str_len, &len, 0, ENT_QUOTES, "UTF-8", double_encode TSRMLS_CC);
	RETVAL_STRINGL(replaced, len, 0);
}
/* }}} */

/* {{{ proto string drupal_static(string arg)
   Experimental implementation of drupal_static */
PHP_FUNCTION(drupal_static_experimental)
{
	zval *name = NULL;
	zval *zdeft = NULL;
	zend_bool reset = 0;
	zval **drawer;
	zval *new_drawer;
	int arg_len;
	zval *zdata = DRUPAL_EXTENSION_G(drupal_static_zdata);
	zval *zdefault = DRUPAL_EXTENSION_G(drupal_static_zdefault);
	char *key = NULL;
	int key_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|zb", &name, &zdeft, &reset, &arg_len) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(name) == IS_NULL) {
		// TODO: Reset all variables.
		RETURN_ZVAL(zdata, 0, 0);
	}
	else {
		key = Z_STRVAL_P(name);
		key_len = Z_STRLEN_P(name);
	}

	if (reset) {
		// TODO: Implement reset.
		
	}
	if (zend_hash_find(Z_ARRVAL_P(zdata), key, key_len + 1, (void**)&zdata) == FAILURE) {
		// Initialize the value from the defaults.
		MAKE_STD_ZVAL(new_drawer);
		*new_drawer = *zdeft;
		zval_copy_ctor(new_drawer);
		add_assoc_zval(zdata, key, new_drawer);
		drawer = &new_drawer;
	}
	RETURN_ZVAL(*drawer, 0, 0);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

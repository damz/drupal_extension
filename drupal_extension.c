
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
	PHP_FE(drupal_static,	arginfo_drupal_static)
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
	// ALlocate memory and initialize hash tables for each page request.
	ALLOC_HASHTABLE(drupal_extension_globals->drupal_static_zdata);
	ALLOC_HASHTABLE(drupal_extension_globals->drupal_static_zdefault);

	zend_hash_init(drupal_extension_globals->drupal_static_zdata, 0, NULL, NULL, 0);
	zend_hash_init(drupal_extension_globals->drupal_static_zdefault, 0, NULL, NULL, 0);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(drupal_extension)
{
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

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(drupal_extension)
{
	// Initialize arrays to hold static variables on a per page request basis.
	// Doing this in MINIT will make the same arrays persistent across multiple requests which is probably not wanted.
	ZEND_INIT_MODULE_GLOBALS(drupal_extension, php_drupal_extension_init_globals, NULL)
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(drupal_extension)
{
	zend_hash_destroy(DRUPAL_EXTENSION_G(drupal_static_zdata));
	zend_hash_destroy(DRUPAL_EXTENSION_G(drupal_static_zdefault));

	FREE_HASHTABLE(DRUPAL_EXTENSION_G(drupal_static_zdata));
	FREE_HASHTABLE(DRUPAL_EXTENSION_G(drupal_static_zdefault));

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
PHP_FUNCTION(drupal_static)
{
	zval *name = NULL;
	zval **drawer = NULL;
	zval **default_drawer = NULL;
	zval *new_drawer = NULL;
	zval *zdeft = NULL;
	zend_bool reset = 0;
	HashTable *zdata = DRUPAL_EXTENSION_G(drupal_static_zdata);
	HashTable *zdefault = DRUPAL_EXTENSION_G(drupal_static_zdefault);
	char *key = NULL;
	int key_len = 0;
	int arg_len; // Currently only used for zend_parse_parameters().
	ulong computed_key; 

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|zb", &name, &zdeft, &reset, &arg_len) == FAILURE) {
		RETURN_FALSE;
	}
	
	if (Z_TYPE_P(name) == IS_NULL) {
		// TODO: Finish implementing. Likely need to iterate thru the entire list of keys and call the zval destructor on each value.
		// Calling zend_hash_clean before destructing its zval containers that were tied to PHP script variables will probably lead to problems. 
		RETURN_TRUE;
		zend_hash_clean(zdata);
		zend_hash_copy(zdata, zdefault, NULL, NULL, sizeof(zval *));
		return;
	} else if (Z_TYPE_P(name) != IS_STRING) {
		// name holds the key and must be a string if not NULL. We have to bail out here to prevent segfaults and other horrors.
		convert_to_string(name);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid key (%s) specified. Key must be a string", Z_STRVAL_P(name));
		RETURN_FALSE;
	}

	key = Z_STRVAL_P(name);
	key_len = Z_STRLEN_P(name) + 1; // Hash functions want the entire string size including its trailing NULL character.

	// Pre-compute the key's hash value so we can use the hash quick functions.
	computed_key = zend_get_hash_value(key, key_len);

	if (zend_hash_quick_find(zdata, key, key_len, computed_key, (void **)&drawer) == FAILURE) {
		// drawer should be NULL if we got here, but there was a segfault and re-setting drawer to NULL seemed to fix it.
		drawer = NULL;
		// Handle the case where no default value was given.
		if (zdeft == NULL) {
			// zdeft gets initialized when the caller sets the returned reference to a value (array, scalar, etc).
			zdeft = EG(uninitialized_zval_ptr);
    		} 
		
		zval_add_ref(&zdeft);
		// Create a copy of the default value for the zdata array.
		MAKE_STD_ZVAL(new_drawer);
		*new_drawer = *zdeft;
		zval_copy_ctor(new_drawer);

		// Drawer is now a pointer to the newly stored value.
		zend_hash_quick_add(zdata, key, key_len, computed_key, &new_drawer, sizeof(zval *), (void **)&drawer);
 		zend_hash_quick_add(zdefault, key, key_len, computed_key, &zdeft, sizeof(zval *), NULL);
	}
    
	if (reset) {
		if (zend_hash_quick_find(zdefault, key, key_len, computed_key, (void **)&default_drawer) != FAILURE) {
			// Free memory associated with the to-be-replaced key in the zdata array.
			zval_ptr_dtor(drawer);
			drawer = NULL;

                        // Create a copy of the default value stored for this key and put it in the zdata array.
			MAKE_STD_ZVAL(new_drawer);
			*new_drawer = **default_drawer;
			zval_copy_ctor(new_drawer);
			zend_hash_quick_update(zdata, key, key_len, computed_key, &new_drawer, sizeof(zval *), (void **)&drawer);

			SEPARATE_ZVAL_TO_MAKE_IS_REF(drawer);
			zval_add_ref(drawer);
			*return_value_ptr = *drawer;
			return;
		} else {
			// Shouldn't happen. If the key was stored and the key in the default array is now missing then we have a problem.
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can't reset key (%s). Default value not found!", key);
			RETURN_FALSE;
		}	
	} 

	// Make the stored value a reference so PHP's garbage collector doesn't delete it.
	SEPARATE_ZVAL_TO_MAKE_IS_REF(drawer);
	zval_add_ref(drawer);
	// Return a reference to the stored value.
	*return_value_ptr = *drawer;
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

#ifndef TEST_CMOCKA_H_
#define TEST_CMOCKA_H_

/*
 * cmocka 1.x requires callers to include these standard headers before
 * cmocka.h. cmocka 2.x includes them itself. Keep this wrapper until our
 * minimum supported cmocka version moves to 2.x.
 */
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#endif /* TEST_CMOCKA_H_ */

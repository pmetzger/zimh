#include "test_cmocka.h"

#include "sel32_chan_internal.h"

/* Verify devices without an IOCL queue report no queue without pointer math. */
static void test_ioclq_for_unit_returns_null_without_queue(void **state)
{
    DIB dib = {0};

    (void)state;

    dib.numunits = 2;
    assert_null(sel32_ioclq_for_unit(&dib, 1));
    assert_null(sel32_ioclq_for_unit(NULL, 1));
}

/* Verify the optional IOCL queue lookup returns the requested unit queue. */
static void test_ioclq_for_unit_returns_existing_queue(void **state)
{
    IOCLQ queues[2] = {0};
    DIB dib = {0};

    (void)state;

    dib.ioclq_ptr = queues;
    dib.numunits = 2;

    assert_ptr_equal(sel32_ioclq_for_unit(&dib, 0), &queues[0]);
    assert_ptr_equal(sel32_ioclq_for_unit(&dib, 1), &queues[1]);
}

/* Verify invalid unit indexes do not form out-of-range queue pointers. */
static void test_ioclq_for_unit_rejects_invalid_unit(void **state)
{
    IOCLQ queues[2] = {0};
    DIB dib = {0};

    (void)state;

    dib.ioclq_ptr = queues;
    dib.numunits = 2;

    assert_null(sel32_ioclq_for_unit(&dib, -1));
    assert_null(sel32_ioclq_for_unit(&dib, 2));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_ioclq_for_unit_returns_null_without_queue),
        cmocka_unit_test(test_ioclq_for_unit_returns_existing_queue),
        cmocka_unit_test(test_ioclq_for_unit_rejects_invalid_unit),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

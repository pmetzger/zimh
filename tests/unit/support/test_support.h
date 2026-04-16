#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include <stddef.h>
#include <stdio.h>

#include "sim_defs.h"

const char *simh_test_source_root(void);
const char *simh_test_binary_root(void);
void simh_test_init_device_unit(DEVICE *device, UNIT *unit,
                                const char *dev_name,
                                const char *unit_name, uint32 dev_flags,
                                uint32 unit_flags, uint32 dwidth,
                                uint32 aincr);
int simh_test_join_path(char *buffer, size_t buffer_size, const char *base,
                        const char *relative_path);
int simh_test_fixture_path(char *buffer, size_t buffer_size,
                           const char *relative_path);
int simh_test_make_temp_dir(char *buffer, size_t buffer_size,
                            const char *prefix);
int simh_test_read_file(const char *path, void **data_out, size_t *size_out);
int simh_test_read_fixture(const char *relative_path, void **data_out,
                           size_t *size_out);
int simh_test_write_file(const char *path, const void *data, size_t size);
int simh_test_read_stream(FILE *stream, char **text_out, size_t *size_out);
int simh_test_files_equal(const char *left_path, const char *right_path);
int simh_test_remove_path(const char *path);

#endif

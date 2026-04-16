#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "sim_defs.h"
#include "sim_imd.h"
#include "test_support.h"

struct sim_imd_fixture {
    char temp_dir[1024];
    char image_path[1024];
};

static int make_path(char *buffer, size_t buffer_size, const char *dir_path,
                     const char *leaf_name)
{
    int length;

    length = snprintf(buffer, buffer_size, "%s/%s", dir_path, leaf_name);
    if (length < 0 || (size_t)length >= buffer_size) {
        return -1;
    }

    return 0;
}

static int setup_sim_imd_fixture(void **state)
{
    struct sim_imd_fixture *fixture;

    fixture = calloc(1, sizeof(*fixture));
    assert_non_null(fixture);

    assert_int_equal(simh_test_make_temp_dir(fixture->temp_dir,
                                             sizeof(fixture->temp_dir),
                                             "sim-imd"),
                     0);
    assert_int_equal(make_path(fixture->image_path, sizeof(fixture->image_path),
                               fixture->temp_dir, "image.imd"),
                     0);

    *state = fixture;
    return 0;
}

static int teardown_sim_imd_fixture(void **state)
{
    struct sim_imd_fixture *fixture = *state;

    assert_int_equal(simh_test_remove_path(fixture->temp_dir), 0);
    free(fixture);
    *state = NULL;
    return 0;
}

static void test_disk_open_rejects_blank_image(void **state)
{
    struct sim_imd_fixture *fixture = *state;
    FILE *image;

    image = fopen(fixture->image_path, "wb+");
    assert_non_null(image);

    assert_null(diskOpen(image, 0));

    fclose(image);
}

static void test_disk_open_rejects_corrupt_header(void **state)
{
    static const uint8_t corrupt_image[] = {'I',
                                            'M',
                                            'D',
                                            ' ',
                                            't',
                                            'e',
                                            's',
                                            't',
                                            '\n',
                                            0x1A,
                                            IMD_MODE_500K_FM,
                                            MAX_CYL,
                                            0x00,
                                            0x01,
                                            0x00,
                                            0x01,
                                            SECT_RECORD_NORM,
                                            0xE5};
    struct sim_imd_fixture *fixture = *state;
    FILE *image;

    assert_int_equal(simh_test_write_file(fixture->image_path, corrupt_image,
                                          sizeof(corrupt_image)),
                     0);

    image = fopen(fixture->image_path, "rb");
    assert_non_null(image);

    assert_null(diskOpen(image, 0));

    fclose(image);
}

static void test_disk_open_reads_minimal_single_sector_image(void **state)
{
    struct sim_imd_fixture *fixture = *state;
    uint8_t image_bytes[10 + 5 + 1 + 1 + 128];
    uint8_t sector_data[128];
    uint8_t read_buffer[128];
    uint32 flags;
    uint32 readlen;
    FILE *image;
    DISK_INFO *disk;

    memset(sector_data, 0xE5, sizeof(sector_data));

    memcpy(image_bytes, "IMD test\n\x1A", 10);
    image_bytes[10] = IMD_MODE_500K_FM;
    image_bytes[11] = 0x00;
    image_bytes[12] = 0x00;
    image_bytes[13] = 0x01;
    image_bytes[14] = 0x00;
    image_bytes[15] = 0x01;
    image_bytes[16] = SECT_RECORD_NORM;
    memcpy(&image_bytes[17], sector_data, sizeof(sector_data));

    assert_int_equal(simh_test_write_file(fixture->image_path, image_bytes,
                                          sizeof(image_bytes)),
                     0);

    image = fopen(fixture->image_path, "rb");
    assert_non_null(image);

    disk = diskOpen(image, 0);
    assert_non_null(disk);

    assert_int_equal(disk->ntracks, 1);
    assert_int_equal(imdGetSides(disk), 1);
    assert_int_equal(imdIsWriteLocked(disk), 0);
    assert_int_equal(disk->track[0][0].nsects, 1);
    assert_int_equal(disk->track[0][0].sectsize, 128);
    assert_int_equal(disk->track[0][0].start_sector, 1);

    memset(read_buffer, 0, sizeof(read_buffer));
    assert_int_equal(sectRead(disk, 0, 0, 1, read_buffer, sizeof(read_buffer),
                              &flags, &readlen),
                     SCPE_OK);
    assert_int_equal(flags, 0);
    assert_int_equal(readlen, sizeof(read_buffer));
    assert_memory_equal(read_buffer, sector_data, sizeof(read_buffer));

    assert_int_equal(diskClose(&disk), SCPE_OK);
    assert_null(disk);
    fclose(image);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_disk_open_rejects_blank_image,
                                        setup_sim_imd_fixture,
                                        teardown_sim_imd_fixture),
        cmocka_unit_test_setup_teardown(test_disk_open_rejects_corrupt_header,
                                        setup_sim_imd_fixture,
                                        teardown_sim_imd_fixture),
        cmocka_unit_test_setup_teardown(
            test_disk_open_reads_minimal_single_sector_image,
            setup_sim_imd_fixture, teardown_sim_imd_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

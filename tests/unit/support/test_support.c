#include "test_support.h"

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* Build one filesystem path from a base directory and relative leaf. */
int simh_test_join_path(char *buffer, size_t buffer_size, const char *base,
                        const char *relative_path)
{
    int written;

    written = snprintf(buffer, buffer_size, "%s/%s", base, relative_path);
    if (written < 0 || (size_t) written >= buffer_size) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return 0;
}

static int remove_directory_contents(const char *path)
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(path);
    if (dir == NULL) {
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        char child[PATH_MAX];

        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (snprintf(child, sizeof(child), "%s/%s", path, entry->d_name) >=
            (int) sizeof(child)) {
            closedir(dir);
            errno = ENAMETOOLONG;
            return -1;
        }

        if (simh_test_remove_path(child) != 0) {
            closedir(dir);
            return -1;
        }
    }

    if (closedir(dir) != 0) {
        return -1;
    }

    return 0;
}

const char *simh_test_source_root(void)
{
    return SIMH_TEST_SOURCE_ROOT;
}

const char *simh_test_binary_root(void)
{
    return SIMH_TEST_BINARY_ROOT;
}

/* Initialize a minimal one-unit device for host-side common-code tests. */
void simh_test_init_device_unit(DEVICE *device, UNIT *unit,
                                const char *dev_name,
                                const char *unit_name, uint32 dev_flags,
                                uint32 unit_flags, uint32 dwidth,
                                uint32 aincr)
{
    memset(device, 0, sizeof(*device));
    memset(unit, 0, sizeof(*unit));

    unit->flags = unit_flags;
    unit->uname = (char *)unit_name;
    unit->dptr = device;

    device->name = dev_name;
    device->units = unit;
    device->numunits = 1;
    device->flags = dev_flags;
    device->dwidth = dwidth;
    device->aincr = aincr;
}

int simh_test_fixture_path(char *buffer, size_t buffer_size,
                           const char *relative_path)
{
    char base[PATH_MAX];

    if (simh_test_join_path(base, sizeof(base), simh_test_source_root(),
                            "tests/unit/fixtures") != 0) {
        return -1;
    }

    return simh_test_join_path(buffer, buffer_size, base, relative_path);
}

int simh_test_make_temp_dir(char *buffer, size_t buffer_size,
                            const char *prefix)
{
    int written;

    written = snprintf(buffer, buffer_size, "%s/%s-XXXXXX",
                       simh_test_binary_root(), prefix);
    if (written < 0 || (size_t) written >= buffer_size) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return mkdtemp(buffer) == NULL ? -1 : 0;
}

int simh_test_read_file(const char *path, void **data_out, size_t *size_out)
{
    FILE *file;
    long size;
    void *data;
    size_t bytes_read;

    file = fopen(path, "rb");
    if (file == NULL) {
        return -1;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return -1;
    }

    size = ftell(file);
    if (size < 0) {
        fclose(file);
        return -1;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return -1;
    }

    data = malloc((size_t) size + 1);
    if (data == NULL) {
        fclose(file);
        errno = ENOMEM;
        return -1;
    }

    bytes_read = fread(data, 1, (size_t) size, file);
    if (bytes_read != (size_t) size) {
        free(data);
        fclose(file);
        errno = EIO;
        return -1;
    }

    ((char *) data)[size] = '\0';

    if (fclose(file) != 0) {
        free(data);
        return -1;
    }

    *data_out = data;
    *size_out = (size_t) size;

    return 0;
}

int simh_test_read_fixture(const char *relative_path, void **data_out,
                           size_t *size_out)
{
    char path[PATH_MAX];

    if (simh_test_fixture_path(path, sizeof(path), relative_path) != 0) {
        return -1;
    }

    return simh_test_read_file(path, data_out, size_out);
}

int simh_test_write_file(const char *path, const void *data, size_t size)
{
    FILE *file;
    size_t bytes_written;

    file = fopen(path, "wb");
    if (file == NULL) {
        return -1;
    }

    bytes_written = fwrite(data, 1, size, file);
    if (bytes_written != size) {
        fclose(file);
        errno = EIO;
        return -1;
    }

    return fclose(file) == 0 ? 0 : -1;
}

int simh_test_files_equal(const char *left_path, const char *right_path)
{
    void *left_data;
    void *right_data;
    size_t left_size;
    size_t right_size;
    int result;

    if (simh_test_read_file(left_path, &left_data, &left_size) != 0) {
        return -1;
    }

    if (simh_test_read_file(right_path, &right_data, &right_size) != 0) {
        free(left_data);
        return -1;
    }

    result = left_size == right_size &&
             memcmp(left_data, right_data, left_size) == 0;

    free(left_data);
    free(right_data);

    return result ? 1 : 0;
}

int simh_test_remove_path(const char *path)
{
    struct stat st;

    if (lstat(path, &st) != 0) {
        return -1;
    }

    if (S_ISDIR(st.st_mode)) {
        if (remove_directory_contents(path) != 0) {
            return -1;
        }

        return rmdir(path);
    }

    return unlink(path);
}

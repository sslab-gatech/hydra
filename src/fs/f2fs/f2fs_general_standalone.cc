// SPDX-License-Identifier: MIT

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "f2fs_fuzzer.hh"
#include "utils.hh"

/* usage: 
   ./f2fs_standalone test in_image out_image meta_image
    - generate meta_image from in_image, and recover it back to out_image
 */

int main(int argc, char *argv[]) 
{

  if (argc < 5) {
    fprintf(stderr, "invalid arg\n");
    return 1;
  }

  if (!strcmp(argv[1], "test")) {
    const char *in_path = argv[2];
    const char *out_path = argv[3];
    const char *meta_path = argv[4];

    struct stat st;
    lstat(in_path, &st);

    void *image_buffer = malloc(st.st_size);
    f2fs_fuzzer.general_compress(in_path, image_buffer, meta_path);

    lstat(meta_path, &st);
    
    void *tmp_buffer = malloc(st.st_size);
    int fd = open(meta_path, O_RDONLY);
    if (read(fd, tmp_buffer, st.st_size) != st.st_size)
      FATAL("reading %s failed.", argv[2]);
    close(fd);

    f2fs_fuzzer.general_decompress(tmp_buffer, st.st_size, true);

    f2fs_fuzzer.sync_to_file(out_path);

    free(tmp_buffer);
    free(image_buffer);

  } else if (!strcmp(argv[1], "repro")) {
    const char *in_path = argv[2];
    const char *out_path = argv[3];
    const char *meta_path = argv[4];

    struct stat st;
    lstat(in_path, &st);

    void *image_buffer = malloc(st.st_size);
    f2fs_fuzzer.general_compress(in_path, image_buffer, "meta.img");
    
    lstat(meta_path, &st);

    void *tmp_buffer = malloc(st.st_size);
    int fd = open(meta_path, O_RDONLY);
    if (read(fd, tmp_buffer, st.st_size) != st.st_size)
      FATAL("reading %s failed.", argv[2]);
    close(fd);

    f2fs_fuzzer.general_decompress(tmp_buffer, st.st_size, true);

    f2fs_fuzzer.sync_to_file(out_path);

    free(tmp_buffer);
    free(image_buffer);

  } else {
    fprintf(stderr, "arg not supported!\n");
    return 1;
  }

  return 0;
}

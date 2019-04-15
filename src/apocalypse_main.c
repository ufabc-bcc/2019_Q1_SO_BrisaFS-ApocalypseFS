#include "apocalypse_main.h"

int main(int argc, char *argv[]) {

    printf("Starting ApocalypseFS...\n");
    printf("\t Maximum file size = 1 block = %d bytes\n", BLOCK_SIZE);
    printf("\t Inode size : %lu\n", sizeof(inode));
    printf("\t Maximum capacity of files: %lu\n", MAX_FILES);

    init_apocalypsefs();

    return fuse_main(argc, argv, &fuse_apocalypsefs, NULL);
}

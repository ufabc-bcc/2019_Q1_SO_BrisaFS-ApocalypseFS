#include "apocalypse_main.h"

void sigint_handler(int sig)
{
    printf("\n ApocalypseFS is about to finnish... \n"); 

    char * filepath = getDumpPath();
    printf("Dump file (preview): %s\n", filepath);

    int fd, ret;
    if ((fd = open(filepath, S_IWUSR)) == -1){
        printf("Trying to create persistent file...\n");

        if ((fd = creat(filepath, S_IRUSR | S_IWUSR)) < 0){
            perror("Error in file creation process\n");
            exit(EXIT_FAILURE);
        }

        if ((ret = write(fd, disk, MAX_BLOCKS * BLOCK_SIZE)) == -1){
            perror("Could not write data to disk at the momment!\n");
            exit(EXIT_FAILURE);
            return;
        }
        else if (ftruncate(fd, MAX_BLOCKS * BLOCK_SIZE)){
            perror("Could not truncate file... Finishing...\n");
            exit(EXIT_FAILURE);
        }

        printf("Memmory dumped successfully!\n");
        close(fd);
    }
    else {
        // Dump file already exists!
        if ((ret = write(fd, disk, MAX_BLOCKS * BLOCK_SIZE)) == -1){
            perror("Could not write data to disk at the momment!\n");
            exit(EXIT_FAILURE);
        }

        printf("Memmory dumped successfully!\n");
        close(fd);
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    /* Esta parte "registra" a aplicação para o recebimento e tratação
        de sinais do SO (Especificamente, o CTRL + C) */
    signal(SIGINT, sigint_handler);

    /* Esta estrutura contém os ponteiros para as operações implementadas
    no FS */
    struct fuse_operations fuse_apocalypsefs = {
                                                .create = create_apocalypsefs,
                                                .fsync = fsync_apocalypsefs,
                                                .getattr = getattr_apocalypsefs,
                                                .chmod = chmod_apocalypsefs,
                                                .chown = chown_apocalypsefs,
                                                .mknod = mknod_apocalypsefs,
                                                .open = open_apocalypsefs,
                                                .read = read_apocalypsefs,
                                                .readdir = readdir_apocalypsefs,
                                                .truncate	= truncate_apocalypsefs,
                                                .utimens = utimens_apocalypsefs,
                                                .write = write_apocalypsefs
    };

    printf("Starting ApocalypseFS...\n");
    printf("\t Maximum file size = 1 block = %d bytes\n", BLOCK_SIZE);
    printf("\t Inode size : %lu\n", sizeof(inode));
    printf("\t Maximum capacity of files: %lu\n", MAX_FILES);

    if (!init_apocalypsefs()) return EXIT_FAILURE;

    return fuse_main(argc, argv, &fuse_apocalypsefs, NULL);
}

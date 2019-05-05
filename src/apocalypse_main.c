#include "apocalypse_main.h"

int main(int argc, char *argv[]) {
    /* Esta estrutura contém os ponteiros para as operações implementadas
    no FS */
    struct fuse_operations fuse_apocalypsefs = {
                                                .create = create_apocalypsefs,
                                                .fsync = fsync_apocalypsefs,
                                                .getattr = getattr_apocalypsefs,
                                                .chmod = chmod_apocalypsefs,
                                                .chown = chown_apocalypsefs,
                                                .mknod = mknod_apocalypsefs,
                                                .mkdir = mkdir_apocalypsefs,
                                                .open = open_apocalypsefs,
                                                .read = read_apocalypsefs,
                                                .readdir = readdir_apocalypsefs,
                                                .truncate = truncate_apocalypsefs,
                                                .unlink = unlink_apocalypsefs,
                                                .utimens = utimens_apocalypsefs,
                                                .write = write_apocalypsefs
						
    };

    printf("Starting ApocalypseFS...\n");
    printf("\t Maximum file size = 1 block = %d bytes\n", BLOCK_SIZE);
    printf("\t Inode size : %lu\n", sizeof(inode));
    printf("\t Maximum capacity of files: %u\n", MAX_FILES);

    if (!init_apocalypsefs()) return EXIT_FAILURE;

    /* Inicia a execução do sistema de arquivos sobre o FUSE e aguarda
        até que seja interrompido (ele não encerra a main) */

    int fuse_res = fuse_main(argc, argv, &fuse_apocalypsefs, NULL);

    /* Chama a função responsável por salvar a sessão atual (não é
        necessário tratar o SIGINT) */
    return fuse_res + save_apocalypsefs_instance();
}

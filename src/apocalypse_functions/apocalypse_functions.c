/*
 * Emilio Francesquini <e.francesquini@ufabc.edu.br>
 * 2019-02-03
 *
 * Este código foi criado como parte do enunciado do projeto de
 * programação da disciplina de Sistemas Operacionais na Universidade
 * Federal do ABC. Você pode reutilizar este código livremente
 * (inclusive para fins comerciais) desde que sejam mantidos, além
 * deste aviso, os créditos aos autores e instituições.
 *
 * Licença: CC-BY-SA 4.0
 *
 */

#include "apocalypse_functions.h"

char * getDumpPath(){char *filepath;
    char *filename = "/dump.apocalypse";

    filepath = malloc(strlen(getenv("HOME")) + strlen(filename) + 1);
    strcpy(filepath, getenv("HOME"));
    strcat(filepath, filename);

    return filepath;
}

//-------------------------------------------------------------------

void fill_block (int isuperblock, const char *nome, uint32_t direitos,
                    uint32_t tamanho, uint32_t block, time_t current, const byte *conteudo) {
    char *mnome = (char*)nome;

    strcpy(superblock[isuperblock].name, mnome);
    superblock[isuperblock].rights = direitos;
    superblock[isuperblock].group = getgid();
    superblock[isuperblock].user = getuid();
    superblock[isuperblock].size = tamanho;
    superblock[isuperblock].block = block;
    superblock[isuperblock].rTime = current;
    superblock[isuperblock].isDir = 0;

    if (conteudo != NULL)
        memcpy(disk + DISK_OFFSET(block), conteudo, tamanho);
    else
        memset(disk + DISK_OFFSET(block), 0, tamanho);
}

//-------------------------------------------------------------------

int init_apocalypsefs() {
    disk = calloc (MAX_BLOCKS, BLOCK_SIZE);

    char *filepath = getDumpPath();

    printf("%s\n", filepath);

    int fd, ret;
    struct stat buffer;
    if (!stat(filepath, &buffer)){
        printf("Opening persistent file...\n");
        
        if ((fd = open(filepath, O_RDONLY)) < 0){
            perror("It was not possible to open the existing file.\nClosing application.\n");
            return 0;
        }
        if ((ret = read(fd, disk, MAX_BLOCKS * BLOCK_SIZE)) == -1){
            perror("Could not retrieve data from disk\n");
            return 0;
        }
        
        close(fd);
        printf("Dump file loaded successfully!");
        // Referencia o bloco no disco
        superblock = (inode*) disk; //posição 0
    }
    else{
        // O superbloco continua com 0s e é iniciado normalmente
        superblock = (inode*) disk; //posição 0

        printf("No dump file has been found... Starting from zero\n");
        //Cria um arquivo na mão de boas vindas
        char *nome = "/You're wellcome.txt";
        //Cuidado! pois se tiver acentos em UTF8 uma letra pode ser mais que um byte
        char *conteudo = "Bem vindo ao sistema de arquivos Apocalypse.\n";
        //0 está sendo usado pelo superblock.fileID O primeiro livre é o 1
        fill_block(0, nome, DEFAULT_RIGHTS, strlen(conteudo), 1, time(NULL), (byte*)conteudo);
    }
    return 1;
}

//-------------------------------------------------------------------

int save_apocalypsefs_instance(){
    printf("\n\n ApocalypseFS is about to finish... \n"); 

    char * filepath = getDumpPath();
    printf("Dump file (preview): %s\n", filepath);

    int fd, ret;
    if ((fd = open(filepath, O_WRONLY)) == -1){
        printf("Trying to create persistent file...\n");

        if ((fd = creat(filepath, S_IRUSR | S_IWUSR)) < 0){
            perror("Error in file creation process\n");
            return 10;
        }

        if ((ret = write(fd, disk, MAX_BLOCKS * BLOCK_SIZE)) == -1){
            perror("Could not write data to disk at the moment!\n");
            return 10;
        }
        else if (ftruncate(fd, MAX_BLOCKS * BLOCK_SIZE)){
            perror("Could not truncate file... Finishing...\n");
            return 10;
        }

        printf("Memory dumped successfully!\n");
    }
    else {
        // Dump file already exists!
        if ((ret = write(fd, disk, MAX_BLOCKS * BLOCK_SIZE)) == -1){
            perror("Could not write data to disk at the moment!\n");
            return 10;
        }

        printf("Memory dumped successfully!\n");
    }

    close(fd);
    return 0;
}

//-------------------------------------------------------------------

int compare_name (const char *a, const char *b) {
    char *ma = (char*)a;
    char *mb = (char*)b;
    //Joga fora barras iniciais
    while (ma[0] != '\0' && ma[0] == '/')
        ma++;
    while (mb[0] != '\0' && mb[0] == '/')
        mb++;
    //Cuidado! Pode ser necessário jogar fora também barras repetidas internas
    //quando tiver diretórios
    return strcmp(ma, mb) == 0;
}

//-------------------------------------------------------------------

int chmod_apocalypsefs(const char *path, mode_t mode, struct fuse_file_info *fi){
    //Diretório raiz
    if (strcmp(path, "/") == 0) {
        return -EPERM; // Operação não permitida na raíz do diretório
    }

    //Busca arquivo na lista de inodes
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block != 0 //Bloco sendo usado
            && compare_name(superblock[i].name, path)) { //Nome bate
            superblock[i].rights = mode;
            return 0; //OK, arquivo encontrado
        }
    }

    return -ENOENT;
}

//-------------------------------------------------------------------

int chown_apocalypsefs(const char *path, uid_t user_id, gid_t group_id,
                        struct fuse_file_info *fi){
    //Diretório raiz
    if (strcmp(path, "/") == 0) {
        return -EPERM; // Operação não permitida na raíz do diretório
    }

    //Busca arquivo na lista de inodes
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block != 0 //Bloco sendo usado
            && compare_name(superblock[i].name, path)) { //Nome bate
            if (user_id != -1) superblock[i].user = user_id; //Se for igual à -1, não foi fornecido
            if (group_id != -1) superblock[i].group = group_id; //Se for igual à -1, não foi fornecido
            return 0; //OK, arquivo encontrado
        }
    }

    return -ENOENT;
}

//-------------------------------------------------------------------

int getattr_apocalypsefs(const char *path, struct stat *stbuf,
                           struct fuse_file_info *fi) {
    memset(stbuf, 0, sizeof(struct stat));
    
    //Diretório raiz
    if (!strcmp(path, "/")) {
        stbuf->st_mode = __S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    //Busca arquivo na lista de inodes
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block != 0 //Bloco sendo usado
            && compare_name(superblock[i].name, path)) { //Nome bate
            stbuf->st_mode = superblock[i].isDir ? __S_IFDIR | 0755 : __S_IFREG | superblock[i].rights;
            stbuf->st_nlink = 1;
            stbuf->st_uid = superblock[i].user;
            stbuf->st_gid = superblock[i].group;
            stbuf->st_size = superblock[i].size;
            stbuf->st_ctime = superblock[i].rTime; // Last status change time
            stbuf->st_mtime = superblock[i].rTime; // Modification time
            // stbuf->st_atime = superblock[i].rTime; // Access time
            return 0; //OK, arquivo encontrado
        }
    }

    //Erro arquivo não encontrado
    return -ENOENT;
}

//-------------------------------------------------------------------

int readdir_apocalypsefs(const char *path, void *buf, fuse_fill_dir_t filler,
                           off_t offset, struct fuse_file_info *fi,
                           enum fuse_readdir_flags flags) {
    (void) offset;
    (void) fi;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    if (!strcmp(path, "/")){
        for (int i = 0; i < MAX_FILES; i++) {
            if (superblock[i].block //Bloco ocupado!
                && isFromDir("/", superblock[i].name)) { //Pertence ao diretório[
                filler(buf, superblock[i].name + 1, NULL, 0, 0);
            }
        }
    }
    else{
        int i;
        for (i = 0; i < MAX_FILES; ++i)
            if (superblock[i].isDir &&
                !strcmp(path, superblock[i].name)) break;

        unsigned int *block = (unsigned int *)(disk + DISK_OFFSET(superblock[i].block));
        for (int k = 0; k < MAX_DIR_CONTENTS; ++k){
            if (block[k] == (unsigned int)END_DIR) break;
            filler(buf, baseFileName(superblock[block[k]].name), NULL, 0, 0);
        }
    }

    return 0;
}

//-------------------------------------------------------------------

int open_apocalypsefs(const char *path, struct fuse_file_info *fi) {
    return 0;
}

//-------------------------------------------------------------------

int read_apocalypsefs(const char *path, char *buf, size_t size,
                        off_t offset, struct fuse_file_info *fi) {

    //Procura o arquivo
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block == 0) //block vazio
            continue;
        if (compare_name(path, superblock[i].name)) {//achou!
            size_t len = superblock[i].size;
            if (offset >= len) {//tentou ler além do fim do arquivo
                return 0;
            }
            if (offset + size > len) {
                memcpy(buf,
                       disk + DISK_OFFSET(superblock[i].block),
                       len - offset);
                return len - offset;
            }

            memcpy(buf,
                   disk + DISK_OFFSET(superblock[i].block), size);
            return size;
        }
    }
    //Arquivo não encontrado
    return -ENOENT;
}

//-------------------------------------------------------------------

int write_apocalypsefs(const char *path, const char *buf, size_t size,
                         off_t offset, struct fuse_file_info *fi) {

    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block == 0) { //block vazio
            continue;
        }
        if (compare_name(path, superblock[i].name)) {//achou!
            // Cuidado! Não checa se a quantidade de bytes cabe no arquivo!
            memcpy(disk + DISK_OFFSET(superblock[i].block) + offset, buf, size);
            superblock[i].size = offset + size;
            return size;
        }
    }
    //Se chegou aqui não achou. Entao cria
    //Acha o primeiro block vazio
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block == 0) {//ninguem usando
            fill_block (i, path, DEFAULT_RIGHTS, size, i + 1, time(NULL), buf);
            setDirProperties(path, i);
            return size;
        }
    }

    return -EIO;
}


//-------------------------------------------------------------------

int truncate_apocalypsefs(const char *path, off_t size, struct fuse_file_info *fi) {
    if (size > BLOCK_SIZE)
        return EFBIG;

    //procura o arquivo
    int findex = -1;
    for(int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block != 0
            && compare_name(path, superblock[i].name)) {
            findex = i;
            break;
        }
    }
    if (findex != -1) {// arquivo existente
        superblock[findex].size = size;
        return 0;
    } else {
        // Arquivo novo
        //Acha o primeiro block vazio
        for (int i = 0; i < MAX_FILES; i++) {
            if (superblock[i].block == 0) {//ninguem usando
                fill_block (i, path, DEFAULT_RIGHTS, size, i + 1, time(NULL), NULL);
                setDirProperties(path, i);
                break;
            }
        }
    }
    return 0;
}

//-------------------------------------------------------------------

int mknod_apocalypsefs(const char *path, mode_t mode, dev_t rdev) {
    if (S_ISREG(mode)) {
        //So aceito criar arquivos normais
        //Acha o primeiro block vazio
        for (int i = 0; i < MAX_FILES; i++) {
            if (!superblock[i].block) {//ninguem usando
                fill_block (i, path, mode & 0x1FF, 0, i + 1, time(NULL), NULL);
                setDirProperties(path, i);
                return 0;
            }
        }
        return ENOSPC;
    }
    return EINVAL;
}

//----------------------------------------------------MAX_FILES + 2---------------

int mkdir_apocalypsefs(const char *path, mode_t mode){
    // So aceito criar diretorios normais
    for (int i = 0; i < MAX_FILES; ++i){
        if (!superblock[i].block){
            fill_block (i, path, mode & 0x1FF, 0, i + 1, time(NULL), NULL);
            setDirProperties(path, i);
            superblock[i].isDir = 1;
        
            unsigned int * block = (unsigned int *)(disk + DISK_OFFSET(superblock[i].block));
            block[0] = (unsigned int)END_DIR;
            return 0;
        }
    }
    return ENOSPC;
}

//-------------------------------------------------------------------

int fsync_apocalypsefs(const char *path, int isdatasync,
                         struct fuse_file_info *fi) {
    // Joga tudo o que está na memória no disco
    return save_apocalypsefs_instance() ? -1 : 0;
}

//-------------------------------------------------------------------

int utimens_apocalypsefs(const char *path, const struct timespec ts[2],
                           struct fuse_file_info *fi) {
    // Cuidado! O sistema BrisaFS não aceita horários. O seu deverá aceitar!
    return 0;
}

//-------------------------------------------------------------------

int create_apocalypsefs(const char *path, mode_t mode,
                          struct fuse_file_info *fi) {
    //Acha o primeiro block vazio
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block == 0) {//ninguem usando
            fill_block (i, path, mode & 0x1FF, 0, i + 1, time(NULL), NULL);

            // TODO: Procurar por máximo de arquivos antes de criar
            setDirProperties(path, i);
            return 0;
        }
    }
    return ENOSPC;
}

//-------------------------------------------------------------------

int unlink_apocalypsefs(const char *path){
    char parentDir[255], started = 0;
    strcpy(parentDir, getDirectoryPath(path));
    int k = strlen(parentDir) ? -1 : -2;

	//Procura o arquivo
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block == 0) //block vazio
            continue;

        // Ja tenta detectar o inode do diretorio pai caso não seja o raiz
        if (k == -1 && !strcmp(parentDir, superblock[i].name)
                && superblock[i].isDir){
            k = i; // Achou o pai!
        }
        else if (compare_name(path, superblock[i].name)) {//achou!
            // Se nao for diretorio raiz
            if (k != -2){
                // Se ainda nao achou o pai, continua procurando
                for(int s = i; s < MAX_FILES && k == -1; s++)
                    if (!strcmp(parentDir, superblock[s].name) && superblock[s].isDir) k = s; // Achou o pai!
                if (k == -1) return -EINVAL;
                
                // Corrige as referências de inode no diretório pai
                unsigned int *block = (unsigned int*)(disk + DISK_OFFSET(superblock[k].block));
                for (int j = 0; j < MAX_DIR_CONTENTS - 1; ++j){
                    if (block[j] == (unsigned int)i) started = 1;
                    if (started) block[j] = block[j + 1];
                    if (block[j] == END_DIR) break;
                }
            }

            superblock[i].block = 0;
            return 0;
        }
    }
    return -ENOENT;
}

//-------------------------------------------------------------------

int rmdir_apocalypsefs(const char *path){
    char parentDir[255], started = 0;
    strcpy(parentDir, getDirectoryPath(path));
    int k = strlen(parentDir) ? -1 : -2;

    //Procura o diretório
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block == 0) //block vazio
            continue;
enhanced
        // Ja tenta detectar o inode do diretorio pai caso não seja o raiz
        if (k == -1 && !strcmp(parentDir, superblock[i].name)
                && superblock[i].isDir){
            k = i; // Achou o pai!
        }
        else if (compare_name(path, superblock[i].name)
                    && superblock[i].isDir) {//achou!
            // Se nao for diretorio raiz
            if (k != -2){
                // Se ainda nao achou o pai, continua procurando
                for(int s = i; s < MAX_FILES && k == -1; s++)
                    if (!strcmp(parentDir, superblock[s].name) && superblock[s].isDir) k = s; // Achou o pai!
                if (k == -1) return -EINVAL;
                
                // Corrige as referências de inode no diretório pai
                unsigned int *block = (unsigned int*)(disk + DISK_OFFSET(superblock[k].block));
                for (int j = 0; j < MAX_DIR_CONTENTS - 1; ++j){
                    if (block[j] == (unsigned int)i) started = 1;
                    if (started) block[j] = block[j + 1];
                    if (block[j] == END_DIR) break;
                }
            }

            superblock[i].block = 0;
            return 0;
        }
    }
    return -ENOENT;
}

//-------------------------------------------------------------------
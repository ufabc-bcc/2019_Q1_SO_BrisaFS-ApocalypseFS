
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
    uint32_t to_read = 0;
    uint32_t node = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block == 0) //block vazio
            continue;
        if (compare_name(path, superblock[i].name)) {//achou!
            
            node = i;
            size_t len = superblock[i].size;
            size = offset + size > len ? len - offset:size;//garantia que não lerá além do final do arquivo
            to_read = size;

            uint32_t offset_holder = offset;
            for(int j = 0; j<offset_holder/BLOCK_SIZE;j++){
                node = superblock[node].next;
                offset -= BLOCK_SIZE;
            }


            while(to_read){
                if(offset+to_read <= BLOCK_SIZE ){
                    memcpy(buf, disk + DISK_OFFSET(superblock[node].block) + offset, to_read);
                    return size;
                }else{
                    memcpy(buf, disk + DISK_OFFSET(superblock[node].block)+offset, BLOCK_SIZE - offset);
                    to_read -= BLOCK_SIZE - offset;
                    buf += BLOCK_SIZE - offset;
                    node = superblock[node].next;
                    offset = 0;
                }
                
            }

            memcpy(buf,
                   disk + DISK_OFFSET(superblock[node].block), to_read);
            return size;
        }
    }
    //Arquivo não encontrado
    return -ENOENT;
}


//-------------------------------------------------------------------

int write_apocalypsefs(const char *path, const char *buf, size_t size,
                         off_t offset, struct fuse_file_info *fi) {
    uint32_t block = 0;
    uint32_t write = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block == 0) { //block vazio
            continue;
        }
        if (compare_name(path, superblock[i].name)) {
            block = i;
            
            uint32_t offset_holder = offset;
            for(int j = 0; j<offset_holder/BLOCK_SIZE;j++){
                if(superblock[block].next){
                    block = superblock[block].next;
                    offset -= BLOCK_SIZE;
                }
            }
            //chegando aqui significa que percorreu o offset até chegar no bloco que vai de fato começar a escrever
            write = size;
            while(write){
                if(offset+write <= BLOCK_SIZE ){
                    memcpy(disk + DISK_OFFSET(superblock[block].block) + offset, buf, write);
                    write = 0;
                }else{
                    memcpy(disk + DISK_OFFSET(superblock[block].block) + offset, buf, BLOCK_SIZE - offset);
                    write -= BLOCK_SIZE - offset;
                    buf += BLOCK_SIZE - offset;
                    offset = 0;

                    if(superblock[block].next){//caso onde não é o último bloco do arquivo
                        block = superblock[block].next;
                    }else{
                        superblock[block].next = find_empty_inode();
                        block = superblock[block].next;

                        strcpy(superblock[block].name, "\0"); // o nome ser nulo que indicará que o bloco
                        superblock[block].size = size;
                        superblock[block].block = block + 1;
                        superblock[block].rTime = 0;
                        superblock[block].next = 0;
        
                    }
                }  
            }
            superblock[i].size = offset + size > superblock[i].size ? offset + size:superblock[i].size;
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

//-------------------------------------------------------------------

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
            setDirProperties(path, i);
            return 0;
        }
    }
    return ENOSPC;
}

//-------------------------------------------------------------------

int rename_apocalypsefs(const char *oldPath, const char *newPath, unsigned int flags){
    int oldParIndex, newParIndex;
    unsigned int inodeIndex = MAX_FILES + 3;

    char parentDirOld[PATH_SIZE]; // Directory path of the old name
    char parentDirNew[PATH_SIZE]; // Directory path of the new name
    char newPathAlt[PATH_SIZE];   // This one is safer to be used

    strcpy(newPathAlt, newPath);
    strcpy(parentDirOld, getDirectoryPath(oldPath));
    strcpy(parentDirNew, getDirectoryPath(newPath));

    oldParIndex = strlen(parentDirOld) ? -1 : -2;
    newParIndex = strlen(parentDirNew) ? -1 : -2;

	// Verifica se o novo nome ja existe
    // Verifica se o novo diretorio pai existe
    // Busca pelo atual diretorio pai
    // Verifica se o arquivo existe
    for (unsigned int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block == 0) //block vazio
            continue;

        // Achou dir pai novo
        if (!strcmp(superblock[i].name, parentDirOld)
            && superblock[i].isDir && oldParIndex == -1)
            oldParIndex = i;

        // Achou dir pai atual
        if (!strcmp(superblock[i].name, parentDirNew)
            && superblock[i].isDir && newParIndex == -1)
            newParIndex = i;

        // Achou inode
        if (compare_name(oldPath, superblock[i].name))
            inodeIndex = i;

        // Achou arquivo ja existente
        if (compare_name(superblock[i].name, newPath)){
            //Se ja existe e nao pode substituir, cai fora
            if (flags == 0x1) return EEXIST;

            int max = strlen(newPathAlt);
            newPathAlt[max++] = '1';
            newPathAlt[max] = '\0';
        }
    }

    if (newParIndex == -1 || oldParIndex == -1 || inodeIndex == MAX_FILES + 3) return ENOENT;

    if (oldParIndex != -2){
        // Corrige as referências de inode no diretório pai
        unsigned int *block = (unsigned int*)(disk + DISK_OFFSET(superblock[oldParIndex].block));
        char started = 0;
        for (int j = 0; j < MAX_DIR_CONTENTS - 1; ++j){
            if (block[j] == inodeIndex) started = 1;
            if (started) block[j] = block[j + 1];
            if (block[j] == END_DIR) break;
        }
    }

    strcpy(superblock[inodeIndex].name, newPathAlt);
    setDirProperties(newPathAlt, inodeIndex);
    
    return 0;
}

//-------------------------------------------------------------------

int unlink_apocalypsefs(const char *path){
    char parentDir[PATH_SIZE], started = 0;
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
            // Libera os blocos adicionais tambem
            while(superblock[i].next){
                i = superblock[i].next;
                superblock[i].block = 0;
            }
            return 0;
        }
    }
    return -ENOENT;
}

//-------------------------------------------------------------------

int rmdir_apocalypsefs(const char *path){
    char parentDir[PATH_SIZE], started = 0;
    strcpy(parentDir, getDirectoryPath(path));
    int k = strlen(parentDir) ? -1 : -2;

    //Procura o diretório
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i].block == 0) //block vazio
            continue;
            
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
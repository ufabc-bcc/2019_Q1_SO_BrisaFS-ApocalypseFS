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

void fill_block (int isuperblock, const char *nome, uint16_t direitos,
                     uint16_t tamanho, uint16_t block, const byte *conteudo) {
    char *mnome = (char*)nome;
    //Joga fora a(s) barras iniciais
    while (mnome[0] != '\0' && mnome[0] == '/')
        mnome++;

    strcpy(superblock[isuperblock].name, mnome);
    superblock[isuperblock].rights = direitos;
    superblock[isuperblock].size = tamanho;
    superblock[isuperblock]block = block;
    if (conteudo != NULL)
        memcpy(disk + DISK_OFFSET(block), conteudo, tamanho);
    else
        memset(disk + DISK_OFFSET(block), 0, tamanho);
}

//-------------------------------------------------------------------

void init_apocalypsefs() {
    disk = calloc (MAX_BLOCKS, BLOCK_SIZE);
    superblock = (inode*) disk; //posição 0
    //Cria um arquivo na mão de boas vindas
    char *nome = "UFABC SO 2019.txt";
    //Cuidado! pois se tiver acentos em UTF8 uma letra pode ser mais que um byte
    char *conteudo = "Adoro as aulas de SO da UFABC!\n";
    //0 está sendo usado pelo superblock. O primeiro livre é o 1
    fill_block(0, nome, DEFAULT_RIGHTS, strlen(conteudo), 1, (byte*)conteudo);
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

static int getattr_apocalypsefs(const char *path, struct stat *stbuf,
                           struct fuse_file_info *fi) {
    memset(stbuf, 0, sizeof(struct stat));

    //Diretório raiz
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    //Busca arquivo na lista de inodes
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i]block != 0 //Bloco sendo usado
            && compare_name(superblock[i].name, path)) { //Nome bate

            stbuf->st_mode = S_IFREG | superblock[i].rights;
            stbuf->st_nlink = 1;
            stbuf->st_size = superblock[i].size;
            return 0; //OK, arquivo encontrado
        }
    }

    //Erro arquivo não encontrado
    return -ENOENT;
}

//-------------------------------------------------------------------

static int readdir_apocalypsefs(const char *path, void *buf, fuse_fill_dir_t filler,
                           off_t offset, struct fuse_file_info *fi,
                           enum fuse_readdir_flags flags) {
    (void) offset;
    (void) fi;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i]block != 0) { //Bloco ocupado!
            filler(buf, superblock[i].name, NULL, 0, 0);
        }
    }

    return 0;
}

//-------------------------------------------------------------------

static int open_apocalypsefs(const char *path, struct fuse_file_info *fi) {
    return 0;
}

//-------------------------------------------------------------------

static int read_apocalypsefs(const char *path, char *buf, size_t size,
                        off_t offset, struct fuse_file_info *fi) {

    //Procura o arquivo
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i]block == 0) //block vazio
            continue;
        if (compare_name(path, superblock[i].name)) {//achou!
            size_t len = superblock[i].size;
            if (offset >= len) {//tentou ler além do fim do arquivo
                return 0;
            }
            if (offset + size > len) {
                memcpy(buf,
                       disk + DISK_OFFSET(superblock[i]block),
                       len - offset);
                return len - offset;
            }

            memcpy(buf,
                   disk + DISK_OFFSET(superblock[i]block), size);
            return size;
        }
    }
    //Arquivo não encontrado
    return -ENOENT;
}

//-------------------------------------------------------------------

static int write_apocalypsefs(const char *path, const char *buf, size_t size,
                         off_t offset, struct fuse_file_info *fi) {

    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i]block == 0) { //block vazio
            continue;
        }
        if (compare_name(path, superblock[i].name)) {//achou!
            // Cuidado! Não checa se a quantidade de bytes cabe no arquivo!
            memcpy(disk + DISK_OFFSET(superblock[i]block) + offset, buf, size);
            superblock[i].size = offset + size;
            return size;
        }
    }
    //Se chegou aqui não achou. Entao cria
    //Acha o primeiro block vazio
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i]block == 0) {//ninguem usando
            fill_block (i, path, DEFAULT_RIGHTS, size, i + 1, buf);
            return size;
        }
    }

    return -EIO;
}


//-------------------------------------------------------------------

static int truncate_apocalypsefs(const char *path, off_t size, struct fuse_file_info *fi) {
    if (size > BLOCK_SIZE)
        return EFBIG;

    //procura o arquivo
    int findex = -1;
    for(int i = 0; i < MAX_FILES; i++) {
        if (superblock[i]block != 0
            && compare_name(path, superblock[i].name)) {
            findex = i;
            break;
        }
    }
    if (findex != -1) {// arquivo existente
        superblock[findex].size = size;
        return 0;
    } else {// Arquivo novo
        //Acha o primeiro block vazio
        for (int i = 0; i < MAX_FILES; i++) {
            if (superblock[i]block == 0) {//ninguem usando
                fill_block (i, path, DEFAULT_RIGHTS, size, i + 1, NULL);
                break;
            }
        }
    }
    return 0;
}

//-------------------------------------------------------------------

static int mknod_apocalypsefs(const char *path, mode_t mode, dev_t rdev) {
    if (S_ISREG(mode)) { //So aceito criar arquivos normais
        //Cuidado! Não seta os direitos corretamente! Veja "man 2
        //mknod" para instruções de como pegar os direitos e demais
        //informações sobre os arquivos
        //Acha o primeiro block vazio
        for (int i = 0; i < MAX_FILES; i++) {
            if (superblock[i]block == 0) {//ninguem usando
                fill_block (i, path, DEFAULT_RIGHTS, 0, i + 1, NULL);
                return 0;
            }
        }
        return ENOSPC;
    }
    return EINVAL;
}

//-------------------------------------------------------------------

static int fsync_apocalypsefs(const char *path, int isdatasync,
                         struct fuse_file_info *fi) {
    //Como tudo é em memória, não é preciso fazer nada.
    // Cuidado! Você vai precisar jogar tudo que está só em memóri no disk
    return 0;
}

//-------------------------------------------------------------------

static int utimens_apocalypsefs(const char *path, const struct timespec ts[2],
                           struct fuse_file_info *fi) {
    // Cuidado! O sistema BrisaFS não aceita horários. O seu deverá aceitar!
    return 0;
}

//-------------------------------------------------------------------

static int create_apocalypsefs(const char *path, mode_t mode,
                          struct fuse_file_info *fi) {
    //Cuidado! Está ignorando todos os parâmetros. O seu deverá
    //cuidar disso Veja "man 2 mknod" para instruções de como pegar os
    //direitos e demais informações sobre os arquivos Acha o primeiro
    //block vazio
    for (int i = 0; i < MAX_FILES; i++) {
        if (superblock[i]block == 0) {//ninguem usando
            fill_block (i, path, DEFAULT_RIGHTS, 0, i + 1, NULL);
            return 0;
        }
    }
    return ENOSPC;
}

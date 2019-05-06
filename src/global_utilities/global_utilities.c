#include "global_utilities.h"


char * getDumpPath(){char *filepath;
    char *filename = "/dump.apocalypse";

    filepath = malloc(strlen(getenv("HOME")) + strlen(filename) + 1);
    strcpy(filepath, getenv("HOME"));
    strcat(filepath, filename);

    return filepath;
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

uint32_t find_empty_inode(){
    for (uint32_t i = 0; i < MAX_FILES; i++)
        if (!superblock[i].block) //ninguem usando
            return i;
    return 0;
}

//-------------------------------------------------------------------

void fill_block (int isuperblock, const char *nome, uint32_t direitos,
                    uint32_t tamanho, uint32_t block, time_t current, const byte *conteudo) {
    char *mnome = (char*)nome;
    uint32_t empty;

    strcpy(superblock[isuperblock].name, mnome);
    superblock[isuperblock].rights = direitos;
    superblock[isuperblock].group = getgid();
    superblock[isuperblock].user = getuid();
    superblock[isuperblock].size = tamanho;
    superblock[isuperblock].block = block;
    superblock[isuperblock].aTime = current;
    superblock[isuperblock].cTime = current;
    superblock[isuperblock].mTime = current;
    superblock[isuperblock].isDir = 0;
    superblock[isuperblock].next = 0;
    

    if (conteudo != NULL){
        while(tamanho > BLOCK_SIZE){
            empty = find_empty_inode();

            memcpy(disk + DISK_OFFSET(block), conteudo, BLOCK_SIZE);
            superblock[isuperblock].next = empty;

            isuperblock = empty;
            tamanho -= BLOCK_SIZE;
            block = empty + 1;

            strcpy(superblock[isuperblock].name, "\0"); // o nome ser nulo que indicará que o bloco
            superblock[isuperblock].rights = direitos;
            superblock[isuperblock].group = 0;
            superblock[isuperblock].user = 0;
            superblock[isuperblock].size = tamanho;
            superblock[isuperblock].block = block;
            superblock[isuperblock].aTime = 0;
            superblock[isuperblock].cTime = 0;
            superblock[isuperblock].mTime = 0;
            superblock[isuperblock].isDir = 0;
            superblock[isuperblock].next = 0;
        }
        memcpy(disk + DISK_OFFSET(block), conteudo, tamanho);
    }
    else
        memset(disk + DISK_OFFSET(block), 0, tamanho);
}

//-------------------------------------------------------------------
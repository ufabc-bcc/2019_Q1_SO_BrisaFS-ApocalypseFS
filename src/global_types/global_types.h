#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#include <time.h>

#include "../global_macros/global_macros.h"

typedef char byte;

/* Um inode guarda todas as informações relativas a um arquivo como
   por exemplo nome, direitos, tamanho, block inicial, ... */
typedef struct {
    char name[PATH_SIZE];
    uint32_t user;
    uint32_t group;
    uint32_t rights;
    uint32_t size;
    uint32_t block;

    uint32_t next;
    time_t aTime;
    time_t cTime;
    time_t mTime;

    byte isDir;
} inode;

/* Disco - A variável abaixo representa um disk que pode ser acessado
   por blocks de tamanho BLOCK_SIZE com um total de MAX_BLOCKS. Você
   deve substituir por um arquivo real e assim persistir os seus
   dados! */
byte *disk;

//guarda os inodes dos arquivos
inode *superblock;

#endif // GLOBAL_TYPES_H

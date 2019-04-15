#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#include <time.h>

#include "../global_types/global_types.h"

typedef char byte;

/* Um inode guarda todas as informações relativas a um arquivo como
   por exemplo nome, direitos, tamanho, block inicial, ... */
typedef struct {
    char name[250];
    uint16_t rights;
    uint16_t size;
    uint16_t block;

    time_t time;
} inode;

/* Disco - A variável abaixo representa um disk que pode ser acessado
   por blocks de tamanho BLOCK_SIZE com um total de MAX_BLOCKS. Você
   deve substituir por um arquivo real e assim persistir os seus
   dados! */
byte *disk;

//guarda os inodes dos arquivos
inode *superblock;

#endif // #GLOBAL_TYPES_H

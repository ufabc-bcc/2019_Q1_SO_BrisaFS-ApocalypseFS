#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#include "../global_types/global_types.h"

typedef char byte;

/* Um inode guarda todas as informações relativas a um arquivo como
   por exemplo nome, direitos, tamanho, bloco inicial, ... */
typedef struct {
    char nome[250];
    uint16_t direitos;
    uint16_t tamanho;
    uint16_t bloco;
} inode;

/* Disco - A variável abaixo representa um disco que pode ser acessado
   por blocos de tamanho TAM_BLOCO com um total de MAX_BLOCOS. Você
   deve substituir por um arquivo real e assim persistir os seus
   dados! */
byte *disco;

//guarda os inodes dos arquivos
inode *superbloco;

#endif // #GLOBAL_TYPES_H

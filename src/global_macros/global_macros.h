#ifndef GLOBAL_MACROS_H
#define GLOBAL_MACROS_H

/* Tamnanho do bloco do dispositivo */
#define TAM_BLOCO 4096
/* A atual implementação utiliza apenas um bloco para todos os inodes
   de todos os arquivos do sistema. Ou seja, cria um limite rígido no
   número de arquivos e tamanho do dispositivo. */
#define MAX_FILES (TAM_BLOCO / sizeof(inode))
/* 1 para o superbloco e o resto para os arquivos. Os arquivos nesta
   implementação também tem apenas 1 bloco no máximo de tamanho. */
#define MAX_BLOCOS (1 + MAX_FILES)
/* Parte da sua tarefa será armazenar e recuperar corretamente os
   direitos dos arquivos criados */
#define DIREITOS_PADRAO 0644

#define DISCO_OFFSET(B) (B * TAM_BLOCO)

#endif // GLOBAL_MACROS_H

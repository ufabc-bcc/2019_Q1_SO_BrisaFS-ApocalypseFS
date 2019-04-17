#ifndef GLOBAL_MACROS_H
#define GLOBAL_MACROS_H

// Defines compatible FUSE version
#define FUSE_USE_VERSION 31

/* Tamnanho do block do dispositivo */
#define BLOCK_SIZE 4096
/* A atual implementação utiliza apenas um block para todos os inodes
   de todos os arquivos do sistema. Ou seja, cria um limite rígido no
   número de arquivos e tamanho do dispositivo. */
#define MAX_FILES (BLOCK_SIZE / sizeof(inode))
/* 1 para o superblock e o resto para os arquivos. Os arquivos nesta
   implementação também tem apenas 1 block no máximo de tamanho. */
#define MAX_BLOCKS (1 + MAX_FILES)
// UNIX default rights
#define DEFAULT_RIGHTS 0644

#define DISK_OFFSET(B) (B * BLOCK_SIZE)

#endif // GLOBAL_MACROS_H

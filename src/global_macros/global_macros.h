#ifndef GLOBAL_MACROS_H
#define GLOBAL_MACROS_H

// Defines compatible FUSE version
#define FUSE_USE_VERSION 31

/* Tamnanho do block do dispositivo */
#define BLOCK_SIZE 4096
/* Quantidade máxima de arquivos. */
#define MAX_FILES 1024UL
/* 1 para o superblock e o resto para os arquivos. Os arquivos nesta
   implementação também tem apenas 1 block no máximo de tamanho. */
#define MAX_BLOCKS ((unsigned long)(MAX_FILES * sizeof(inode) / BLOCK_SIZE) + 1 + MAX_FILES)
// UNIX default rights
#define DEFAULT_RIGHTS 0644

#define DISK_OFFSET(B) ((MAX_BLOCKS - B) * BLOCK_SIZE)

#endif // GLOBAL_MACROS_H

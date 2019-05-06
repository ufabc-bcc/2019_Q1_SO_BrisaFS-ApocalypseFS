#ifndef GLOBAL_MACROS_H
#define GLOBAL_MACROS_H

/**
 * Defines compatible FUSE version
 */
#define FUSE_USE_VERSION 31

/**
 * Tamanho do bloco do dispositivo
 */
#define BLOCK_SIZE 4096

/**
 * Tamanho maximo do caminho do arquivo
 */
#define PATH_SIZE 250

/**
 * Quantidade máxima de arquivos.
 */
#define MAX_FILES 65536U

/**
 * Marca o final do diretório.
 */
#define END_DIR MAX_FILES + 2

/**
 * 1 para o superblock e o resto para os arquivos. Os arquivos nesta
 * implementação também tem apenas 1 block no máximo de tamanho.
 */
#define MAX_BLOCKS ((unsigned long)(MAX_FILES * sizeof(inode) / BLOCK_SIZE) + 1 + MAX_FILES)

/**
 * Define a quantidade máxima de arquivos e diretírios dentro
 * de um diretório
 */
#define MAX_DIR_CONTENTS (BLOCK_SIZE / sizeof(unsigned int))

/*
 * UNIX default rights
 */
#define DEFAULT_RIGHTS 0644

/**
 * Macro para calculo do inicio de enderecamento do bloco
 */
#define DISK_OFFSET(B) ((MAX_BLOCKS - B) * BLOCK_SIZE)

#endif // GLOBAL_MACROS_H

#ifndef GLOBAL_UTILITIES_H
#define GLOBAL_UTILITIES_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>

// Inclui cabeçalhos locais necessarios
#include "../global_macros/global_macros.h"
#include "../global_types/global_types.h"

/**
 * Devolve 1 caso representem o mesmo nome ou 0 cc
 */
int compare_name(const char *a, const char *b);

/**
 * Retorna o caminho para o arquivo dump
 */
char * getDumpPath();

/**
 * Retorna o proximo inode vazio
 * ATENCAO: Embora 0 seja um resultado valido, ele pode
 * representar um vazio (voce tera que fazer esta checagem)
 */
uint32_t find_empty_inode();

/**
 *  Preenche os campos do superblock de índice isuperblock
 */
void fill_block (int isuperblock, const char *nome, uint32_t direitos,
                     uint32_t tamanho, uint32_t block, time_t time, const byte *conteudo);

#endif //GLOBAL_UTILITIES_H
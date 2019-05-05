#ifndef DIR_FUNCTIONS_H
#define DIR_FUNCTIONS_H

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
 * Retorna caminho para o diretório pai do arquivo
 * especificado em "path".
 * O arquivo pode ser um arquivo regular ou um diretório
 */

char * getDirectoryPath(const char *path);

/**
 *  (Booleano) Verifica se o arquivo pertence ao diretório indicado
 *  Retorna 0, caso não pertença ao diretório
 */

int isFromDir(const char * dirPath, const char *filePath);

/**
 * Associa o arquivo ao diretório pai (deve ser invocado em todas
 * as funções onde um novo arquivo é criado [create, write,
 * truncate, etc...]).
 * 
 * @return : retorna 1 caso não consiga encontrar o diretorio pai
 */

int setDirProperties(const char *filePath, unsigned int index);

/**
 * Retorna o nome base do arquivo ou diretório
 */

char * baseFileName(const char *filePath);

#endif // !DIR_FUNCTIONS_H
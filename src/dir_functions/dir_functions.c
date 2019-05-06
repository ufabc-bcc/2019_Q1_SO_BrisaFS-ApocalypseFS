#include "dir_functions.h"

//-------------------------------------------------------------------

char * getDirectoryPath(const char *path){
    static char dirName[255];
    int i, n = strlen(path) - 1;

    while(n){
        if (path[n] == '/') break;
        n--;
    }

    for (i = 0; i < n; ++i)
        dirName[i] = path[i];
    dirName[i] = '\0';

    return dirName;
}

//-------------------------------------------------------------------

int isFromDir(const char *dirPath, const char *filePath){
    // Se não começar com o mesmo prefixo então e falso
    if (strncmp(dirPath, filePath, strlen(dirPath))) return 0;

    int n = strlen(filePath) - 1;
    while(n){
        if (filePath[n] == '/') break;
        n--;
    }

    // Aqui os prefixos ja sao iguais
    // Se tiverem o mesmo tamanho, entao retorna true
    return strlen(dirPath) == (n + 1);
}

//-------------------------------------------------------------------

int setDirProperties(const char *filePath, unsigned int index){
    char directory[255];
    strcpy(directory, getDirectoryPath(filePath));

    // Se for a raiz, nao precisa de bind
    if (!strcmp(directory, "/")) return 0;

    for (int i = 0; i < MAX_FILES; ++i){
        if (!strcmp(superblock[i].name, directory) && superblock[i].isDir){ // achou o pai
            unsigned int *block = (unsigned int *)(disk + DISK_OFFSET(superblock[i].block));
            for (int j = 0; j < MAX_DIR_CONTENTS; ++j){ // procura por bloco vazio
                if (block[j] == END_DIR){
                    if (j == MAX_DIR_CONTENTS - 1) return 2;

                    block[j + 1] = END_DIR;
                    block[j] = index;
                    return 0;
                }
            }
        }
    }

    // Nao encontrado
    return 1;
}

//-------------------------------------------------------------------

char * baseFileName(const char *filePath){
    static char baseName[255];
    int n = strlen(filePath) - 1;

    while(n){
        if (filePath[n] == '/') break;
        n--;
    }

    strcpy(baseName, filePath + n + 1);
    return baseName;
}

//-------------------------------------------------------------------
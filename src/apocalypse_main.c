/*
 * Emilio Francesquini <e.francesquini@ufabc.edu.br>
 * 2019-02-03
 *
 * Este código foi criado como parte do enunciado do projeto de
 * programação da disciplina de Sistemas Operacionais na Universidade
 * Federal do ABC. Você pode reutilizar este código livremente
 * (inclusive para fins comerciais) desde que sejam mantidos, além
 * deste aviso, os créditos aos autores e instituições.
 *
 * Licença: CC-BY-SA 4.0
 *
 */

#include "apocalypse_main.h"
#include "global_macros/global_macros.h"
#include "global_types/global_types.h"

int main(int argc, char *argv[]) {

    printf("Iniciando o ApocalypseFS...\n");
    printf("\t Tamanho máximo de arquivo = 1 bloco = %d bytes\n", TAM_BLOCO);
    printf("\t Tamanho do inode: %lu\n", sizeof(inode));
    printf("\t Número máximo de arquivos: %lu\n", MAX_FILES);

    init_apocalypsefs();

    return fuse_main(argc, argv, &fuse_apocalypsefs, NULL);
}

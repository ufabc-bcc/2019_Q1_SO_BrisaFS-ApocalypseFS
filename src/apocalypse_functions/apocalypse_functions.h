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

#ifndef APOCALYPSE_FUNCTIONS_H
#define APOCALYPSE_FUNCTIONS_H

#include <unistd.h>

#include "../global_macros/global_macros.h"
#include "../global_types/global_types.h"

/* Preenche os campos do superblock de índice isuperblock */
void fill_block (int isuperblock, const char *nome, uint16_t direitos,
                     uint16_t tamanho, uint16_t block, const byte *conteudo);

/* Para persistir o FS em um disk representado por um arquivo, talvez
   seja necessário "formatar" o arquivo pegando o seu tamanho e
   inicializando todas as posições (ou apenas o(s) superblock(s))
   com os valores apropriados */
void init_apocalypsefs();

/* Devolve 1 caso representem o mesmo nome e 0 cc */
int compare_name(const char *a, const char *b);

/* A função getattr_apocalypsefs devolve os metadados de um arquivo cujo
   caminho é dado por path. Devolve 0 em caso de sucesso ou um código
   de erro. Os atributos são devolvidos pelo parâmetro stbuf */
static int getattr_apocalypsefs(const char *path, struct stat *stbuf,
                           struct fuse_file_info *fi);


/* Devolve ao FUSE a estrutura completa do diretório indicado pelo
   parâmetro path. Devolve 0 em caso de sucesso ou um código de
   erro. Atenção ao uso abaixo dos demais parâmetros. */
static int readdir_apocalypsefs(const char *path, void *buf, fuse_fill_dir_t filler,
                           off_t offset, struct fuse_file_info *fi,
                           enum fuse_readdir_flags flags);

/* Abre um arquivo. Caso deseje controlar os arquvos abertos é preciso
   implementar esta função */
static int open_apocalypsefs(const char *path, struct fuse_file_info *fi);

/* Função chamada quando o FUSE deseja ler dados de um arquivo
   indicado pelo parâmetro path. Se você implementou a função
   open_apocalypsefs, o uso do parâmetro fi é necessário. A função lê size
   bytes, a partir do offset do arquivo path no buffer buf. */
static int read_apocalypsefs(const char *path, char *buf, size_t size,
                        off_t offset, struct fuse_file_info *fi);

/* Função chamada quando o FUSE deseja escrever dados em um arquivo
   indicado pelo parâmetro path. Se você implementou a função
   open_apocalypsefs, o uso do parâmetro fi é necessário. A função escreve
   size bytes, a partir do offset do arquivo path no buffer buf. */
static int write_apocalypsefs(const char *path, const char *buf, size_t size,
                         off_t offset, struct fuse_file_info *fi);

/* Altera o tamanho do arquivo apontado por path para tamanho size
   bytes */
static int truncate_apocalypsefs(const char *path, off_t size, struct fuse_file_info *fi);

/* Cria um arquivo comum ou arquivo especial (links, pipes, ...) no caminho
   path com o modo mode*/
static int mknod_apocalypsefs(const char *path, mode_t mode, dev_t rdev);

/* Sincroniza escritas pendentes (ainda em um buffer) em disk. Só
   retorna quando todas as escritas pendentes tiverem sido
   persistidas */
static int fsync_apocalypsefs(const char *path, int isdatasync,
                         struct fuse_file_info *fi);

/* Ajusta a data de acesso e modificação do arquivo com resolução de nanosegundos */
static int utimens_apocalypsefs(const char *path, const struct timespec ts[2],
                           struct fuse_file_info *fi);


/* Cria e abre o arquivo apontado por path. Se o arquivo não existir
   cria e depois abre*/
static int create_apocalypsefs(const char *path, mode_t mode,
                          struct fuse_file_info *fi);

/* Esta estrutura contém os ponteiros para as operações implementadas
   no FS */
static struct fuse_operations fuse_apocalypsefs = {
                                              .create = create_apocalypsefs,
                                              .fsync = fsync_apocalypsefs,
                                              .getattr = getattr_apocalypsefs,
                                              .mknod = mknod_apocalypsefs,
                                              .open = open_apocalypsefs,
                                              .read = read_apocalypsefs,
                                              .readdir = readdir_apocalypsefs,
                                              .truncate	= truncate_apocalypsefs,
                                              .utimens = utimens_apocalypsefs,
                                              .write = write_apocalypsefs
};

#endif // APOCALYPSE_FUNCTIONS_H

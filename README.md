# Apocalypse FS

## Testes

- O projeto, atualmente, está sendo desenvolvido e testado nas distros Fedora e Ubuntu.
- O repositório principal poderá conter bugs, porém, o código sempre será compilável. Para acesso às versões mais recentes, alterne para a *branch* **`unstable-dev`**.

## Desenvolvedores

- Mauro Mascarenhas de Araújo

## Baixando, compilando e executando

Para um tutorial completo (e genérico), basta acessar o site: [git - the simple guide - no deep shit!](http://rogerdudler.github.io/git-guide/)

Detalhes específicos do projeto disponíveis em: [Projeto de Programação](http://professor.ufabc.edu.br/~e.francesquini/2019.q1.so/projeto/)

### Baixando

Vá até o diretório base em que deseja clonar o projeto e digite os seguintes comandos:

	$ git clone https://github.com/ufabc-bcc/2019_Q1_SO_BrisaFS-ApocalypseFS.git
	$ cd 2019_Q1_SO_BrisaFS-ApocalypseFS
 
Caso você deseje tentar compilar o código mais recente (poderão haver falhas) execute:

	$ git checkout unstable-dev

Isto irá trocar a `branch` atual, fornecendo os arquivos mais atualizados (porém ainda em desenvolvimento).

### Compilando

Antes de tudo, instale as bibliotecas necessárias (note que, normalmente, o ambiente de desenvolvimento C já está instalado por padrão):

##### Debian e derivados (Ubuntu, Elementary, Deepin, etc.)

	$ sudo apt-get install gcc make fuse libfuse-dev

##### RHEL e derivados (Fedora, CentOS, etc.)

	$ sudo dnf install gcc make gcc make fuse3 fuse3-devel

Para compilar, você tem duas opções:

1. Utilizar o make (mais fácil)
2. Utilizar os comandos GCC

#### Utilizando o make

Uma vez dentro do diretório `./src/`, para compilar, basta utilizar seguinte o comando em seu shell:

	$ make

Caso nenhum erro corra, aparecerá apenas a mensagem **Concluído!** . Pronto! o projeto já foi compilado.

#### Utilizando os comandos GCC diretamente

Execute o comando de compilação:

	$ gcc -Wall -Werror -O3 -g apocalypse_functions/apocalypse_functions.c apocalypse_main.c -o apocalypsefs.run `pkg-config fuse3 --cflags --libs`

### Executando

Para executar, novamente, você tem duas opções:
1. Utilizar o make
2. Inicializar diretamente

#### Utilizando o make

Ainda no diretório `./src/`, basta executar o seguinte comando:

	$ make run

Ele tentará criar automaticamente um diretório de montagem na pasta `tmp`. Caso você deseje especificar o local de montagem, basta definir a variável `MOUNT`:

Ainda no diretório `./src/`, basta executar o seguinte comando:

	$ make run MOUNT=<MOUNT_PATH>

Onde `<MOUNT_PATH>` é o caminho para o ponto de montagem do sistema de arquivos.

#### Inicializando diretamente

Caso nenhum erro tenha ocorrido no processo de compilação, basta executar os seguintes comandos para rodar o programa:

	$ mkdir <MOUNT_PATH>
	$ ./apocalypsefs.run -f -d -s <MOUNT_PATH>

Onde `<MOUNT_PATH>` é o caminho para o ponto de montagem do sistema de arquivos.

## Projeto de Programação - ApocalypseFS

Maiores informações à respeito da base do projeto disponíveis em : [Projeto base](base_implementation/).

![ApocalypseFS](figs/apocalypse.jpg)

# Apocalypse FS

## Testes

- O projeto, atualmente, está sendo desenvolvido e testado nas distros Fedora e Ubuntu.
- O repositório principal poderá conter bugs, porém, o código sempre será compilável. Para acesso às versões mais recentes, alterne para a *branch* `**unstable-dev**`.

## Desenvolvedores

- Mauro Mascarenhas de Araújo

## Baixando, compilando e executando

Para um tutorial completo (e genérico), basta acessar o site: [git - the simple guide - no deep shit!](http://rogerdudler.github.io/git-guide/)
Detalhes específicos do projeto disponíveis em: [Projeto de Programação](http://professor.ufabc.edu.br/~e.francesquini/2019.q1.so/projeto/)

*AVISO : * Assim que o projeto estiver com um bom andamento, implementaremos o `make` para compilá-lo

### Baixando

Vá até o diretório base em que deseja clonar o projeto e digite os seguintes comandos:

	git clone https://github.com/ufabc-bcc/2019_Q1_SO_BrisaFS-ApocalypseFS.git
	cd 2019_Q1_SO_BrisaFS-ApocalypseFS
 
Caso você deseje tentar compilar o código mais recente (poderão haver falhas) execute:

	git checkout unstable-dev

Isto irá trocar a `branch` atual, fornecendo os arquivos mais atualizados (porém ainda em desenvolvimento).

### Compilando

Instale as bibliotecas necessárias (note que, normalmente, o ambiente de desenvolvimento já está instalado por padrão):

##### Debian e derivados (Ubuntu, Elementary, Deepin, etc.)
	$ sudo apt-get install gcc make fuse libfuse-dev

##### RHEL e derivados (Fedora, etc.)
	$ sudo dnf install gcc make gcc make fuse3 fuse3-devel

Execute o comando de compilação:

	$ gcc -Wall -Werror -O3 -g apocalypse_functions/apocalypse_functions.c apocalypse_main.c -o apocalypsefs `pkg-config fuse3 --cflags --libs`

### Executando

Caso nenhum erro tenha ocorrido no processo de compilação, basta executar os seguintes comandos para rodar o programa:

	$ mkdir /tmp/apocalypse_mount
	$ ./apocalypsefs -f -d -s /tmp/apocalypse_mount
	
Caso prefira, você pode alterar o ponto de montagem (diretório) do apocalypsefs.

## Projeto de Programação - ApocalypseFS

Maiores informações à respeito da base do projeto disponíveis em : [Projeto base](base_implementation/).

![ApocalypseFS](figs/apocalypse.jpg)

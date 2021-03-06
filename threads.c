
/*
** Código feito para utilizar multi-thread em aplicações externas.
** Lê arquivo de texto linha por linha e abre passando como parâmetro.
** Obs: Multi-thread.
**
** Autor: Constatine.
** Data: Julho de 2015.
** 
** Compilar...
**  Windows: gcc -std=c99 threads.c -o threads.exe
**  Linux: gcc -std=c99 -pthread threads.c -o threads
*/

#define WINDOWS_USER // Usuários Linux comente essa linha.
#define MAXLIMIT 256

#ifdef WINDOWS_USER
	#define sleep Sleep
	#include <Windows.h>
#else
	#include <pthread.h>
	#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int index;
	char *line;
	char *command;
} param_t;

typedef struct {
	int counter_a;
	int counter_b ;
	int counter_c;
} thread_t;
thread_t *thread;

static void core (const void *tparam);
static unsigned int file_exists (const char *filename);

int main (int argc, char ** argv) {	
	if (argc != 4) {
		printf("\n Use: %s input_list number_of_threads script_name\n", argv[0]);
		printf("  Ex: %s list.txt 10 script.php\n", argv[0]);
		exit(0);
	}
	
	if (!file_exists(argv[3])) {
		printf("File not found: %s\n", argv[3]);
		exit(0);
	}
	
	if ((thread = (thread_t *) malloc(sizeof(thread_t))) != NULL) {
		thread->counter_a = atoi(argv[2]);
		thread->counter_b = 1;
		thread->counter_c = 0;
	} else {
		printf("Error to alloc thread struct.\n");
		exit(0);
	}
	
	if (thread->counter_a == 0)
		thread->counter_a = 1;
	
	FILE *fp = NULL;
	char line [MAXLIMIT];
	memset(line, '\0', MAXLIMIT);

	if ((fp = fopen(argv[1], "r")) != NULL) {
		while (fgets(line, MAXLIMIT-1, fp)) {
			for (int a=0; line[a] != '\0'; a++)
				if (line[a] == '\n') {
					line[a] = '\0';
					break;
				}
			
			if (thread->counter_a) {
				param_t *param = (param_t *) malloc(sizeof(param_t)); 
				param->line = (char *) malloc((strlen(line) * sizeof(char)) + MAXLIMIT);
				memset(param->line, '\0', (strlen(line) * sizeof(char)) + MAXLIMIT);
				memcpy(param->line, line, strlen(line));
				param->command = (char *) malloc((strlen(argv[3]) * sizeof(char)) + MAXLIMIT);
				memset(param->command, '\0', (strlen(argv[3]) * sizeof(char)) + MAXLIMIT);
				memcpy(param->command, argv[3], strlen(argv[3]));
				param->index = thread->counter_c;
				thread->counter_b++;
#ifdef WINDOWS_USER
				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) core, (void *) param, 0, 0);
#else 
				pthread_t td;
				pthread_create(&td, NULL, core, (void *) param);
#endif
				thread->counter_a--;
			}

			while (1) {
				sleep(10);
				if (thread->counter_a) break;
			}
			
			thread->counter_c++;
			memset(line, '\0', MAXLIMIT);
		}
		fclose(fp);
	}

	while (1) {
		sleep(10);
		if (thread->counter_b == 1) break;
	}

	return 0;
}

static void core (const void *tparam) {
	param_t *param = (param_t *) tparam;
	printf("[%d] -> %s\n", param->index, param->line);
	char buffer [MAXLIMIT*2];
	sprintf(buffer, "php %s %d %s", param->command, param->index, param->line);
	system(buffer);
	thread->counter_a++;
	thread->counter_b--;
}

static unsigned int file_exists (const char *filename) {
	if (!filename) return 0;
	FILE *fp = NULL;
	if ((fp = fopen(filename, "r")) != NULL) {
		fclose(fp);
		return 1;
	}
	return 0;
}

/* EOF. */

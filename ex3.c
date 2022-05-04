///////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define N 50000

char genome[N];
char sequence[N];
int n_threads;
int n_reads1;
int n_reads2;
int startPoint[N];
int endPoint[N];
int n_occurrences;
int finishThreads;

void* sequence_scanner (void *i) {

	int value = *(int *) i;
	int value_sequence = 0;

//	printf("reads1 %d\t threads %d\t value %d\n", n_reads1, n_threads, value);

//	printf("ciao\t%d-%d\n", n_reads1/n_threads*(value-1), n_reads1/n_threads*value);

	for ( int j = n_reads1/n_threads*(value-1); j < n_reads1/n_threads*value; j++ ) {

		if ( (genome[j] == sequence[value_sequence]) && (value_sequence == 0) ) {
//			printf("start point %c\t%c\t%d\n", genome[j], sequence[value_sequence], value_sequence);
			value_sequence++;
			startPoint[n_occurrences] = j;
		} else if ( (genome[j] == sequence[value_sequence]) && (value_sequence != 0) ) {
//			printf("middle point %c\t%c\t%d\n", genome[j], sequence[value_sequence], value_sequence);
			value_sequence++;
		} else {
//			printf("no point %c\t%c\t%d\n", genome[j], sequence[value_sequence], value_sequence);
			value_sequence = 0;
		}

		if ( value_sequence == n_reads2-1 ) {
//			printf("startPoint %d\tendPoint %d\n", startPoint[n_occurrences], endPoint[n_occurrences]);
			endPoint[n_occurrences] = j;
			n_occurrences++;
			value_sequence = 0;
		}

		if ( j == n_reads1/n_threads*value ) {
			finishThreads++;
		}

	}

	pthread_exit(NULL);

}

int main (int argc, char* argv[]) {

	int err;
	int var = 1;
	FILE *fp_genome, *fp_sequence;

	//stringFound = 0;
	finishThreads = 0;
	n_occurrences = 0;

	printf("Hello! This is the genoma scanning program!\n");

	fp_genome = fopen("genome.txt", "r");

	if ( fp_genome == NULL ) {
		perror("Error in opening genome file!\n");
		exit(errno);
	}

	n_reads1 = fscanf(fp_genome, "%s", genome);

	fseek(fp_genome, 0L, SEEK_END);
	n_reads1 = ftell(fp_genome);

//	printf("n_reads1 %d\n", n_reads1);

	fp_sequence = fopen("sequence.txt", "r");

	if ( fp_sequence == NULL ) {
		perror("Error in opening sequence file!\n");
		exit(errno);
	}

	n_reads2 = fscanf(fp_sequence, "%s", sequence);

	fseek(fp_sequence, 0L, SEEK_END);
	n_reads2 = ftell(fp_sequence);

//	printf("n_reads2 %d\n", n_reads2);

	printf("How many parallel threads do you want to create?\n");
	scanf("%d", &n_threads);
	printf("\n");

	pthread_t tid[n_threads];

	for ( int i = 1; i < n_threads + 1; i++ ) {
//		printf("i %d\n", i);
		err = pthread_create(&tid[i-1], NULL, sequence_scanner, &i);
		sleep(0.1);
		if ( err != 0 ) {
			perror("Error in creating thread!\n");
			exit(errno);
		}

	}

	for ( int l = 0; l < n_threads; l++ ) {
		pthread_join(tid[l], NULL);
//		printf("tid %ld\n", tid[l]);
	}

	for ( int k = 1; k < n_occurrences + 1; k++ ) {
		printf("%d-Sequence match:\t%d - %d\n", k, startPoint[k-1], endPoint[k-1]);
	}

	printf("\nProgram ended!\n");

	fclose(fp_genome);
	fclose(fp_sequence);

	exit(0);

}


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "defs.h"
#include "list_level_hash.h"

#define SAMPLES_TO_COLLECT   10000000
#define RAND_NUM_UPPER_BOUND   100000
#define NUM_SEED_STREAMS            4

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "Rash",                  /* Team name */

    "Ryan Santhirarajan",                    /* First member full name */
    "998461338",                 /* First member student number */
    "ryan.santhirarajan@mail.utoronto.ca",                 /* First member email address */

    "Ashkan Parcham Kashani",                           /* Second member full name */
    "998446105",                           /* Second member student number */
    "ashkan.parchamkashani@mail.utoronto.ca"                            /* Second member email address */
};

unsigned num_threads;
unsigned samples_to_skip;
pthread_mutex_t lock[RAND_NUM_UPPER_BOUND];

class sample;

class sample {
    unsigned my_key;
    public:
    sample *next;
    unsigned count;

    sample(unsigned the_key){my_key = the_key; count = 0;};
    unsigned key(){return my_key;}
    void print(FILE *f){printf("%d %d\n",my_key,count);}
};

// This instantiates an empty hash table
// it is a C++ template, which means we define the types for
// the element and key value here: element is "class sample" and
// key value is "unsigned".  
hash<sample,unsigned> h;

//
void *collect_sample (void* idx);


int  
main (int argc, char* argv[]){
    int i,j,k;
    int rnum;
    unsigned key;
    sample *s;


    // Print out team information
    printf( "Team Name: %s\n", team.team );
    printf( "\n" );
    printf( "Student 1 Name: %s\n", team.name1 );
    printf( "Student 1 Student Number: %s\n", team.number1 );
    printf( "Student 1 Email: %s\n", team.email1 );
    printf( "\n" );
    printf( "Student 2 Name: %s\n", team.name2 );
    printf( "Student 2 Student Number: %s\n", team.number2 );
    printf( "Student 2 Email: %s\n", team.email2 );
    printf( "\n" );

    // Parse program arguments
    if (argc != 3){
        printf("Usage: %s <num_threads> <samples_to_skip>\n", argv[0]);
        exit(1);  
    }
    sscanf(argv[1], " %d", &num_threads); // not used in this single-threaded version
    sscanf(argv[2], " %d", &samples_to_skip);

    for ( i = 0; i < RAND_NUM_UPPER_BOUND; i++) { 
        if (pthread_mutex_init(&lock[i], NULL) != 0)
        {
            printf("\n mutex init failed\n");
            return 1;
        }
    }

    // initialize a 16K-entry (2**14) hash of empty lists
    h.setup(14);

    //pthread use
    pthread_t thrd[num_threads];
    int index[num_threads];

    for ( i = 0; i < num_threads; i++) {
        //pthread_create(&thrd[i], NULL, collect_sample, (void *) &(samples_to_skip), (void *) &(stream_num));
        index[i] = i;
        pthread_create(&thrd[i], NULL, collect_sample, (void *) &(index[i]));
    }

    for ( i = 0; i < num_threads; i++) {
        pthread_join(thrd[i], NULL);
    }

    for (i = 0; i < RAND_NUM_UPPER_BOUND; i++) {
        pthread_mutex_destroy(&lock[i]);
    }

    // print a list of the frequency of all samples
    h.print();
}

//void collect_sample (void* skips, void* num_stream) { 
void *collect_sample (void* idx) { 

    //unsigned samples_to_skip = *((unsigned *)skips);
    int index = *((int *)idx);
    int i,j,k;
    int rnum;
    unsigned key;
    sample *s;

    int start = index * (NUM_SEED_STREAMS/num_threads);
    // process streams starting with different initial numbers
    for (i=0; i<NUM_SEED_STREAMS/num_threads; i++){
        rnum = start+i;

        // collect a number of samples
        for (j=0; j<SAMPLES_TO_COLLECT; j++){

            // skip a number of samples
            for (k=0; k<samples_to_skip; k++){
                rnum = rand_r((unsigned int*)&rnum);
            }

            // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
            key = rnum % RAND_NUM_UPPER_BOUND;

            pthread_mutex_lock(&lock[key]);
            // if this sample has not been counted before
            if (!(s = h.lookup(key))){

                // insert a new element for it into the hash table
                s = new sample(key);
                h.insert(s);
            }

            // increment the count for the sample
            s->count++;
            pthread_mutex_unlock(&lock[key]);
        }
    }

}

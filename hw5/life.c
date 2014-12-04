/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include "life.h"
#include "util.h"
#include <pthread.h>

/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/

/*****************************************************************************
 * Game of life implementation
 ****************************************************************************/

#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])

#define NUM_THREADS 4

struct ThreadParams {
    int index;
    char* outboard;
    char* inboard;
    int nrows;
    int ncols;
    int gens_max;
};

pthread_barrier_t barr;

void *
do_game_of_life(void* context)
{

    struct ThreadParams *params = context; 
    int index = params->index;
    char* outboard = params->outboard;
    char* inboard = params->inboard;
    const int nrows = params->nrows;
    const int ncols = params->ncols;
    const int gens_max = params->gens_max;

    const int LDA = nrows;
    int curgen, i, j;

    for (curgen = 0; curgen < gens_max; curgen++)
    {
        /* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
        for (j = 0; j < nrows/NUM_THREADS; j++)
        {
            int new_j = j + (index * nrows/NUM_THREADS);
            const int jwest = mod (new_j-1, ncols);
            const int jeast = mod (new_j+1, ncols);
            for (i = 0; i < ncols; i++)
            {
                const int inorth = mod (i-1, nrows);
                const int isouth = mod (i+1, nrows);

                const char neighbor_count = 
                    BOARD (inboard, inorth, jwest) + 
                    BOARD (inboard, inorth, new_j) + 
                    BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, i, jwest) +
                    BOARD (inboard, i, jeast) + 
                    BOARD (inboard, isouth, jwest) +
                    BOARD (inboard, isouth, new_j) + 
                    BOARD (inboard, isouth, jeast);

                BOARD(outboard, i, new_j) = alivep (neighbor_count, BOARD (inboard, i, new_j));
            }
        }
        int rc = pthread_barrier_wait(&barr);
        if (rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
            return NULL;
        }
        SWAP_BOARDS( outboard, inboard );
    }

        //Wait for all threads to complet this generation
    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
}


char* game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max)
{
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */

    //Initialize Barrier
    if (pthread_barrier_init(&barr, NULL, NUM_THREADS)) {
        return NULL;
    }

    pthread_t thrd[NUM_THREADS];
    struct ThreadParams params[NUM_THREADS];

    int i;

    for ( i = 0; i < NUM_THREADS; i++) {
        params[i].index = i;
        params[i].outboard = outboard;
        params[i].inboard = inboard;
        params[i].nrows = nrows;
        params[i].ncols = ncols;
        params[i].gens_max = gens_max;
        int rc = pthread_create(&thrd[i], NULL, do_game_of_life, (void *) &(params[i]));
        //assert(0 == rc);
    }
    
    for ( i = 0; i <NUM_THREADS; i++){
        pthread_join(thrd[i], NULL);
    }
    //all threads have completed all generations
    return inboard;
    
}



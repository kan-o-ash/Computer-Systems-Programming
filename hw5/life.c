/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include <pthread.h>
#include "life.h"
#include "util.h"

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

char *do_game_of_life()
{

    const int LDA = nrows;
    int curgen, i, j;

    for (curgen = 0; curgen < gens_max; curgen++)
    {
        /* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
        for (j = 0; j < nrows; j++)
        {
            const int jwest = mod (j-1, ncols);
            const int jeast = mod (j+1, ncols);
            for (i = 0; i < ncols; i++)
            {
                const int inorth = mod (i-1, nrows);
                const int isouth = mod (i+1, nrows);

                const char neighbor_count = 
                    BOARD (inboard, inorth, jwest) + 
                    BOARD (inboard, inorth, j) + 
                    BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, i, jwest) +
                    BOARD (inboard, i, jeast) + 
                    BOARD (inboard, isouth, jwest) +
                    BOARD (inboard, isouth, j) + 
                    BOARD (inboard, isouth, jeast);

                BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));

            }
        }
        SWAP_BOARDS( outboard, inboard );

    }
    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
    return inboard;
}


    char*
game_of_life (char* outboard, 
        char* inboard,
        const int nrows,
        const int ncols,
        const int gens_max,
        const int t_num)
{
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
    const int num_threads = 4;
    pthread_mutex_t lock[gens_max];

    // initialize locks
    for ( i = 0; i < gens_max; i++) { 
      if (pthread_mutex_init(&lock[i], NULL) != 0)
      {
          printf("\n mutex init failed\n");
          return 1;
      }
    }

    pthread_t thrd[num_threads];
    int index[num_threads];

    for ( i = 0; i < num_threads; i++) {
      index[i] = i;
      rc = pthread_create(&thrd[i], NULL, do_game_of_life, (void *) &(index[i]));
      assert(0 == rc);
    }

    
}



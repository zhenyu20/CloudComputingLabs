#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#include "sudoku.h"

FILE*fp;
char puzzle[128];
  int total_solved = 0;
  int total = 0;

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;


int64_t now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

void *handle(void *arg){
  
  bool (*solve)(int) = solve_sudoku_basic;
  // if (argv[2] != NULL)
  //   if (argv[2][0] == 'a')
  //     solve = solve_sudoku_min_arity;
  //   else if (argv[2][0] == 'c')
  //     solve = solve_sudoku_min_arity_cache;
  //   else if (argv[2][0] == 'd')
      solve = solve_sudoku_dancing_links;

  pthread_mutex_lock(&lock1);
  if(fgets(puzzle, sizeof puzzle, fp) != NULL) {
    pthread_mutex_unlock(&lock1);
    if (strlen(puzzle) >= N) {
      pthread_mutex_lock(&lock2);
      ++total;
      pthread_mutex_unlock(&lock2);
      input(puzzle);
      init_cache();
      //if (solve_sudoku_min_arity_cache(0)) {
      //if (solve_sudoku_min_arity(0))
      //if (solve_sudoku_basic(0)) {
      if (solve(0)) {
        ++total_solved;
        if (!solved())
          assert(0);
      }
      else {
        printf("No: %s", puzzle);
      }
    }
  }
}

int main(int argc, char* argv[])
{
  init_neighbors();

  fp = fopen(argv[1], "r");
  
  int64_t start = now();
  
  pthread_t p[3];
  for(int i=0;i<3;i++){
    pthread_create(&p[i], NULL, handle, NULL);
  }
  


  int64_t end = now();
  double sec = (end-start)/1000000.0;
  printf("%f sec %f ms each %d\n", sec, 1000*sec/total, total_solved);

  return 0;
}


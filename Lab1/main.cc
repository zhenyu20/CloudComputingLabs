#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include "sudoku.h"
pthread_mutex_t lock1,lock2;
int total_solved = 0;
int total = 0;

struct myarg_t
{
  FILE *fp;
  bool (*solve)(int);
};

int64_t now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}
void *solvethread(void *arg)
{
  myarg_t *m = (myarg_t*)arg;
  //char puzzle[128]= "000000010400000000020000000000050407008000300001090000300400200050100000000806000";
  char puzzle[128];
  while (fgets(puzzle, sizeof puzzle, m->fp) != NULL)
  {
    // pthread_t tid = pthread_self();
    // printf("\n thread: [%ld] \n", pthread_self());
    if (strlen(puzzle) >= N)
    {
 
      ++total;
     
      pthread_mutex_lock(&lock1);  
      input(puzzle);
      pthread_mutex_unlock(&lock1);
      init_cache();
      
      bool result = m->solve(0);
    
      if (result)
      {
        ++total_solved;
        if (!solved())
          assert(0);
     
      }
      else
      {
        printf("No: %s", puzzle);
      }
     
    

    }
  }
}

int main(int argc, char *argv[])
{
  init_neighbors();

  myarg_t arg1,arg2,arg3,arg4;
  pthread_t p1,p2,p3,p4,p5;
  FILE *fp = fopen(argv[1], "r");
  arg1.fp=arg2.fp=arg3.fp=arg4.fp=fp;
 
  arg1.solve = solve_sudoku_basic;
  arg2.solve = solve_sudoku_min_arity_cache;
  arg3.solve = solve_sudoku_min_arity;
  arg4.solve = solve_sudoku_dancing_links;
  if (argv[2] != NULL)
    if (argv[2][0] == 'a')
      arg1.solve = solve_sudoku_min_arity;
    else if (argv[2][0] == 'c')
      arg1.solve = solve_sudoku_min_arity_cache;
    else if (argv[2][0] == 'd')
      arg1.solve = solve_sudoku_dancing_links;
  int64_t start = now();
  pthread_create(&p1,NULL,solvethread,&arg1);
  pthread_create(&p2,NULL,solvethread,&arg2);
  pthread_create(&p3,NULL,solvethread,&arg3);
  pthread_create(&p4,NULL,solvethread,&arg4);
  
  // pthread_create(&p3,NULL,solvethread,&arg1);
  // pthread_create(&p4,NULL,solvethread,&arg1);
  // pthread_create(&p5,NULL,solvethread,&arg1);

  pthread_join(p1,NULL);
  pthread_join(p2,NULL);
  pthread_join(p3,NULL);
  pthread_join(p4,NULL);
  // pthread_join(p3,NULL);
  // pthread_join(p4,NULL);
  // pthread_join(p5,NULL);
  int64_t end = now();
  double sec = (end - start) / 1000000.0;
  printf("%f sec %f ms each %d\n", sec, 1000 * sec / total, total_solved);

  return 0;
}

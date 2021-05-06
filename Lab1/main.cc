#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/file.h>
#include <pthread.h>
#include <assert.h>
#include <algorithm>

#include "sudoku_dancing_links.h"
#include "sudoku.h"

#define N 128
#define thread_count 1 //线程数量

int total_solved = 0;
int total = 0;
int get_pos = 0;  //消费者指针位置
int put_pos = 0;  //生产者指针位置
char *buffer[N];  //问题的缓冲区
int num_data = 0; //缓冲区剩的问题的数量
pthread_t thread_sort;
int sort_solve[N][81];
int buffer_num = 0;
int produce_turns = 0;

bool ifNull = false; //文件是否读完

FILE *fp;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;

char *temp; //取数据
char *get() //消费者从缓冲区取问题
{
    temp = buffer[get_pos];
    get_pos = (get_pos + 1) % N; //指针后移一位
                                 // printf("get_pos:%d\n", get_pos);
    num_data--;
    return temp;
}
void *sort1(void *arg)
{

    for (int i = 0; i < buffer_num; i++)
    {
        for (int j = 0; j < 81; j++)
        {
            printf("%d", sort_solve[i][j]);
        }
        printf("\n");
    }
    pthread_exit(NULL);
}
int64_t now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}
bool solved(int (*chess)[9])
{
    for (int row = 0; row < ROW; ++row)
    {
        // check row
        int occurs[10] = {0};
        for (int col = 0; col < COL; ++col)
        {
            int val = chess[row][col];
            assert(1 <= val && val <= NUM);
            ++occurs[val];
        }

        if (std::count(occurs, occurs + 10, 1) != NUM)
            return false;
    }

    for (int col = 0; col < COL; ++col)
    {
        int occurs[10] = {0};
        for (int row = 0; row < ROW; ++row)
        {
            int val = chess[row][col];
            // assert(1 <= val && val <= NUM);
            ++occurs[val];
        }

        if (std::count(occurs, occurs + 10, 1) != NUM)
            return false;
    }

    for (int row = 0; row < ROW; row += 3)
    {
        for (int col = 0; col < COL; col += 3)
        {
            int occurs[10] = {0};
            ++occurs[chess[row][col]];
            ++occurs[chess[row][col + 1]];
            ++occurs[chess[row][col + 2]];
            ++occurs[chess[row + 1][col]];
            ++occurs[chess[row + 1][col + 1]];
            ++occurs[chess[row + 1][col + 2]];
            ++occurs[chess[row + 2][col]];
            ++occurs[chess[row + 2][col + 1]];
            ++occurs[chess[row + 2][col + 2]];

            if (std::count(occurs, occurs + 10, 1) != NUM)
                return false;
        }
    }
    return true;
}
void *execute(void *arg)
{
    int board[81];
    int(*chess)[9] = (int(*)[9])board; //用于检查解是否正确
    int get_pos_t = -1;
    while (1)
    {

        pthread_mutex_lock(&lock1);

        while (num_data == 0 && !ifNull) //缓冲区空了
        {
            pthread_cond_signal(&empty);
            pthread_cond_wait(&full, &lock1);
        }
        if (num_data == 0 && ifNull) //所有任务已完成
        {
            pthread_mutex_unlock(&lock1);
            pthread_exit(NULL); //线程主动结束并退出
        }
        //printf("吃一个 \n");
        total++;
        get_pos_t = get_pos;
        char *data = get();
        
       pthread_mutex_unlock(&lock1);

        for (int cell = 0; cell < 81; ++cell)
        {
            board[cell] = data[cell] - '0';
        }

 
        Dance d(board);
        if (d.solve())
        { //求解

            if (!solved(chess)) //检查解的正确性
                assert(0);

             pthread_mutex_lock(&lock2);
             ++total_solved;
             pthread_mutex_unlock(&lock2);
            for (int i = 0; i < 81; i++)
            {
                sort_solve[get_pos_t][i] = board[i];
            }
        }
        else
        {
            printf("No: %s", data);
        }
    }
}

int main(int argc, char *argv[])
{
    init_neighbors();
    char filename[30];
    int a = scanf("%s", filename);
    fp = fopen(filename, "r");
    //fp = fopen(argv[1], "r");
    //thread_count = strtol(argv[2], NULL, 10);

    for (int i = 0; i < N; i++) //初始化缓冲区
        buffer[i] = (char *)malloc(82 * sizeof(char));
    pthread_t *thread_handles;

    thread_handles = (pthread_t *)malloc(thread_count * sizeof(pthread_t));

    for (int i = 0; i < thread_count; i++) //创建线程
    {
        pthread_create(&thread_handles[i], NULL, execute, NULL);
    }

    int64_t start = now();

    pthread_mutex_lock(&lock1); //生产者的锁
    while (1)
    {

        while (num_data == 128) //缓冲区满了
        {

            buffer_num = N;
            if (produce_turns != 0)//第一次生产不能输出
            {
                pthread_create(&thread_sort, NULL, sort1, NULL);
                pthread_join(thread_sort, NULL);
            }
            pthread_cond_signal(&full);
            pthread_cond_wait(&empty, &lock1);
            produce_turns++;
        }

        if (fgets(buffer[put_pos], 84, fp) != NULL) //从文件读数据
        {
            put_pos = (put_pos + 1) % N;
            num_data++;
        }
        else //文件尾
        {
            if (produce_turns != 0)
            {
                //  printf("%d\n",produce_turns);
                pthread_create(&thread_sort, NULL, sort1, NULL);
                pthread_join(thread_sort, NULL);
            }
            
            buffer_num = num_data;
            //  printf("%d \n",buffer_num);
         
            ifNull = true;
            pthread_cond_broadcast(&full); //广播唤醒所有消费者
            pthread_mutex_unlock(&lock1);
            break;
        }
    }
    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(thread_handles[i], NULL);
    }
    pthread_create(&thread_sort, NULL, sort1, NULL);
    pthread_join(thread_sort, NULL);
    free(thread_handles);
    int64_t end = now();
    double sec = (end - start) / 1000000.0;
    printf("%f sec %f ms each %d\n", sec, 1000 * sec / total, total_solved);
    return 0;
}

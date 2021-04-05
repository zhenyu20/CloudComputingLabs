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
char puzzle[128];
int total_solved = 0;
int total = 0;
int get_pos = 0;     //消费者指针位置
int put_pos = 0;     //生产者指针位置
int thread_count;    //线程数量
char *buffer[N];     //问题的缓冲区
int num_data = 0;    //缓冲区剩的问题的数量
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
    printf("get_pos:%d\n", get_pos);
    num_data--;
    return temp;
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
    while (1)
    {
        pthread_mutex_lock(&lock1);

        while (num_data == 0 && !ifNull) //缓冲区空了
            pthread_cond_wait(&full, &lock1);

        if (num_data == 0 && ifNull) //所有任务已完成
        {
            pthread_mutex_unlock(&lock1);
            pthread_exit(NULL); //线程主动结束并退出
        }
        
        total++;
        char *data = get();
        printf("%d\ngetdata:", num_data);
        for (int i = 0; i < 81; ++i)
        {
            printf("%c", data[i]);
        }
        printf("\n");
        int order = 100 - num_data; //打印顺序
        Dance d(board);
        pthread_cond_signal(&empty); //缓冲区不是满的
        pthread_mutex_unlock(&lock1);
        if (d.solve())
        { //求解
            ++total_solved;
            if (!solved(chess)) //检查解的正确性
                assert(0);
        }
        else
        {
            printf("No: %s", puzzle);
        }
    }
}

int main(int argc, char *argv[])
{
    init_neighbors();
    fp = fopen(argv[1], "r");
    thread_count = strtol(argv[3], NULL, 10);

    for (int i = 0; i < N; i++) //初始化缓冲区
        buffer[i] = (char *)malloc(82 * sizeof(char));

    pthread_t *thread_handles;
    thread_handles = (pthread_t *)malloc(thread_count * sizeof(pthread_t));
    for (int i = 0; i < thread_count; i++) //创建线程
    {
        pthread_create(&thread_handles[i], NULL, execute, NULL);
    }

    int64_t start = now();
    while (1)
    {
        pthread_mutex_lock(&lock1); //生产者的锁
        while (num_data == 128)     //缓冲区满了
        {
            pthread_cond_wait(&empty, &lock1);
        }
        if (fgets(buffer[put_pos], 84, fp) != NULL) //从文件读数据
        {

            printf("buffer:");
            for (int i = 0; i < 81; ++i)
            {
                printf("%c", buffer[put_pos][i]);
            }
            printf("\n");
            //fgetc(fp);
            put_pos = (put_pos + 1) % N;
            printf("put_pos:%d\n", put_pos);
            num_data++;
        }
        else //文件尾
        {
            ifNull = true;
            pthread_mutex_unlock(&lock1);
            pthread_cond_broadcast(&full); //广播唤醒所有消费者
            break;
        }
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&lock1);
    }
    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(thread_handles[i], NULL);
    }
    free(thread_handles);
    int64_t end = now();
    double sec = (end - start) / 1000000.0;
    printf("%f sec %f ms each %d\n", sec, 1000 * sec / total, total_solved);
    return 0;
}

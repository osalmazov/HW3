#include <pthread.h>
#include <stdlib.h>
#include <iostream>

//специальная структура для данных потока
typedef struct{
    int rowN; //номер обрабатываемой строки
    int rowSize; //размер строки
    //указатели на матрицы
    int** array1;
    int** array2;
    int** resArr;
} pthrData;

void* threadFunc(void* thread_data){
    //получаем структуру с данными
    pthrData *data = (pthrData*) thread_data;

    //умнажаем элементы матриц и сохраняем результат
    for(int j = 0; j < data->rowSize; j++){
        for(int k = 0; k < data->rowSize; k++){
            data->resArr[data->rowN][j] += data->array1[data->rowN][k] * data->array2[k][j];
        }
    }
    return NULL;
}

int main(){
    int N;
    std::cin >> N;
    int N_thread;
    int N_div;
    int N_mod;
    std::cin >> N_thread;
    if(N_thread >= N){
        N_div = 1;
        N_thread = N;
        N_mod = 0;
    } else {
        N_div = N / N_thread;
        N_mod = N % N_thread;
    }

    //выделяем память под двумерные массивы
    int** matrix1 = (int**) malloc(N * sizeof(int*));
    int** matrix2 = (int**) malloc(N * sizeof(int*));
    int** resultMatrix = (int**) malloc(N * sizeof(int*));

    //выделяем память под элементы матриц
    for(int i = 0; i < N; i++){
        matrix1[i] = (int*) malloc(N * sizeof(int));
        matrix2[i] = (int*) malloc(N * sizeof(int));
        resultMatrix[i] = (int*) malloc(N * sizeof(int));
    }

    //инициализируем начальными значениями
    srand(time(NULL));
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            matrix1[i][j] = rand() % 100 + 1;
            matrix2[i][j] = rand() % 100 + 1;
            resultMatrix[i][j] = 0;
        }
    }

    //выделяем память под массив идентификаторов потоков
    pthread_t* threads = (pthread_t*) malloc(N_thread * sizeof(pthread_t));
    //сколько потоков - столько и структур с потоковых данных
    pthrData* threadData = (pthrData*) malloc(N_thread * sizeof(pthrData));

    //инициализируем структуры потоков
    int count = 0;
    for(int i = 0; i < N_div; i++){
        for(int j = 0; j < N_thread; j++) {
            threadData[j].rowN = count; // текущая строчка матрицы
            threadData[j].rowSize = N; //ее размер
            threadData[j].array1 = matrix1; // указатель на первую матрицу
            threadData[j].array2 = matrix2; // указатель на вторую матрицу
            threadData[j].resArr = resultMatrix; // указатель на матрицу результата
            pthread_create(&(threads[j]), NULL, threadFunc, &threadData[j]);
            count += 1;
        }
        //ждем пока потоки завершатся чтобы на след итерации
        for(int j = 0; j < N_thread; j++){
            pthread_join(threads[j], NULL);
        }
    }
    double t = clock();
    for(int i = 0; i < N_mod; i++){
        threadData[i].rowN = count;
        threadData[i].rowSize = N;
        threadData[i].array1 = matrix1;
        threadData[i].array2 = matrix2;
        threadData[i].resArr = resultMatrix;
        pthread_create(&(threads[i]), NULL, threadFunc, &threadData[i]);
        count += 1;
    }

    //ожидаем выполнение оставшихся потоков
    for(int i = 0; i < N_mod; i++)
        pthread_join(threads[i], NULL);

    std::cout << "Time1:"<<static_cast<double>((clock() - t) / CLOCKS_PER_SEC) <<std::endl;

    //освобождаем память
    free(threads);
    free(threadData);
    /*
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            std::cout <<  matrix1[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            std::cout <<  matrix2[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            std::cout << resultMatrix[i][j] << " ";
        }
        std::cout << std::endl;
    }*/

    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            resultMatrix[i][j] = 0;
        }
    }
    t = clock();
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            for(int k =0; k < N; k++) {
                resultMatrix[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
    std::cout <<"TIME 2 "<< static_cast<double>((clock() - t) / CLOCKS_PER_SEC) << std::endl;
    std::cout << std::endl;
    /*
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            std::cout << resultMatrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    */

    for(int i = 0; i < N; i++){
        free(matrix1[i]);
        free(matrix2[i]);
        free(resultMatrix[i]);
    }

    free(matrix1);
    free(matrix2);
    free(resultMatrix);
    return 0;
}
/*ПРИМЕР РЕШЕНИЯ С OMP;
#include<omp.h>

int main(){
 int N;
 int N_thread;
 std::cin >> N;
 std::cin >> N_thread;

 int A[N][N];
 int B[N][N];
 int res[N][N];

 for(int i = 0; i < N; i++){
     for(int j = 0; j < N; j++){
         A[i][j] = rand() % 100 + 1;
         B[i][j] = rand() % 100 + 1;
         res[i][j] = 0;
     }
 }
 double wtime = omp_get_wtime();
 omp_set_num_threads(N_thread);
#pragma omp parallel for
 for(int i = 0 ; i < N; i++){
     for(int j = 0; j < N; j++){
         for(int k = 0; k < N; k++){
             res[i][j] += A[i][k] * B[k][j];
         }
     }
 }
 wtime = omp_get_wtime() - wtime;
 std::cout << "Time: " << wtime << std::endl;
}*/
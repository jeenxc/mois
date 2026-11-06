#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF INT_MAX

int **W; 
int *dist; 
int *prev;
int *used; 
int N; 

// Освобождение памяти
void freeMemory() {
    for (int i = 0; i < N; i++)
        free(W[i]);
    free(W);
    free(dist);
    free(prev);
    free(used);
}

void printPathDijkstra(int v) {
    if (v == -1) return;
    if (prev[v] != -1) {
        printPathDijkstra(prev[v]);
        printf(" -> ");
    }
    printf("x%d", v + 1);
}

void dijkstra() {
    for (int i = 0; i < N; i++) {
        dist[i] = INF;
        prev[i] = -1;
        used[i] = 0;
    }
    dist[0] = 0;

    printf("Алгоритм Дейкстры от вершины x1\n");
    printf("================================\n\n");

    for (int iter = 1; iter <= N; iter++) {
        int minDist = INF;
        int u_min = -1;
        for (int i = 0; i < N; i++) {
            if (!used[i] && dist[i] < minDist) {
                minDist = dist[i];
                u_min = i;
            }
        }

        if (u_min == -1) break;
        used[u_min] = 1;

        printf("Итерация %d: выбираем вершину x%d (dist = %d)\n",
               iter, u_min + 1, dist[u_min]);

        for (int j = 0; j < N; j++) {
            if (W[u_min][j] != INF && !used[j]) {
                int newDist = dist[u_min] + W[u_min][j];
                if (newDist < dist[j]) {
                    dist[j] = newDist;
                    prev[j] = u_min;
                    printf("  Обновляем x%d: dist = %d (через x%d)\n",
                           j + 1, dist[j], u_min + 1);
                }
            }
        }
        printf("\n");
    }

    printf("Результаты (Дейкстра):\n");
    printf("--------------------------------------------------\n");
    printf("%-8s %-10s %s\n", "Вершина", "Расстояние", "Путь");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < N; i++) {
        printf("x%-7d ", i + 1);
        if (dist[i] == INF)
            printf("%-10s ", "INF");
        else
            printf("%-10d ", dist[i]);
        printPathDijkstra(i);
        printf("\n");
    }
    printf("\n");
}

void floydWarshall() {
    // Матрица кратчайших расстояний
    int **D = (int **)malloc(N * sizeof(int *));
    // Матрица "следующих" вершин для восстановления пути
    int **next = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        D[i] = (int *)malloc(N * sizeof(int));
        next[i] = (int *)malloc(N * sizeof(int));
    }

    // Инициализация
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            D[i][j] = W[i][j];
            if (i != j && W[i][j] != INF)
                next[i][j] = j;
            else
                next[i][j] = -1;
        }
    }

    printf("Алгоритм Флойда-Уоршалла\n");
    printf("========================\n\n");

    for (int k = 0; k < N; k++) {
        printf("Промежуточная вершина x%d\n", k + 1);
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                // Защита от переполнения при сложении с INF
                if (D[i][k] != INF && D[k][j] != INF) {
                    int newDist = D[i][k] + D[k][j];
                    if (newDist < D[i][j]) {
                        D[i][j] = newDist;
                        next[i][j] = next[i][k];
                        if (i == 0) {  // выводим только для путей от x1
                            printf("  Путь x1 -> x%d через x%d: dist = %d\n",
                                   j + 1, k + 1, D[i][j]);
                        }
                    }
                }
            }
        }
        printf("\n");
    }

    printf("Результаты (Флойд-Уоршалл), пути от x1:\n");
    printf("--------------------------------------------------\n");
    printf("%-8s %-10s %s\n", "Вершина", "Расстояние", "Путь");
    printf("--------------------------------------------------\n");

    for (int j = 0; j < N; j++) {
        printf("x%-7d ", j + 1);
        if (D[0][j] == INF) {
            printf("%-10s ", "INF");
            printf("(пути нет)\n");
        } else {
            printf("%-10d ", D[0][j]);
            // Восстановление пути через матрицу next
            int u = 0;
            printf("x%d", u + 1);
            while (u != j) {
                u = next[u][j];
                printf(" -> x%d", u + 1);
            }
            printf("\n");
        }
    }

    for (int i = 0; i < N; i++) {
        free(D[i]);
        free(next[i]);
    }
    free(D);
    free(next);
}

int main(int argc, char *argv[]) {
    FILE *file;
    int M;
    int u, v, w;

    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_файла>\n", argv[0]);
        return 1;
    }

    file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Ошибка: не удалось открыть файл '%s'\n", argv[1]);
        return 1;
    }

    if (fscanf(file, "%d %d", &N, &M) != 2) {
        fprintf(stderr, "Ошибка чтения первой строки файла\n");
        fclose(file);
        return 1;
    }

    if (N <= 0 || M < 0) {
        fprintf(stderr, "Ошибка: некорректное количество вершин или рёбер\n");
        fclose(file);
        return 1;
    }

    W = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        W[i] = (int *)malloc(N * sizeof(int));
        for (int j = 0; j < N; j++) {
            W[i][j] = (i == j) ? 0 : INF;
        }
    }

    // Чтение рёбер
    for (int i = 0; i < M; i++) {
        if (fscanf(file, "%d %d %d", &u, &v, &w) != 3) {
            fprintf(stderr, "Ошибка чтения ребра %d\n", i + 1);
            freeMemory();
            fclose(file);
            return 1;
        }
        if (u < 1 || u > N || v < 1 || v > N) {
            fprintf(stderr, "Ошибка: вершина %d или %d вне диапазона [1, %d]\n", u, v, N);
            freeMemory();
            fclose(file);
            return 1;
        }
        W[u - 1][v - 1] = w;
        W[v - 1][u - 1] = w;
    }
    fclose(file);

    dist = (int *)malloc(N * sizeof(int));
    prev = (int *)malloc(N * sizeof(int));
    used = (int *)malloc(N * sizeof(int));

    printf("Граф: %d вершин, %d рёбер\n\n", N, M);

    dijkstra();

    floydWarshall();

    freeMemory();
    return 0;
}
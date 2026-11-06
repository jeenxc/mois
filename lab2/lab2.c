#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_V 100
#define MAX_E 5000

typedef struct {
    int n;
    int adj[MAX_V][MAX_V];
    int degree[MAX_V];
    char vertexNames[MAX_V][20];
} Graph;

void initGraph(Graph *g, int n) {
    g->n = n;
    memset(g->adj, 0, sizeof(g->adj));
    memset(g->degree, 0, sizeof(g->degree));
    memset(g->vertexNames, 0, sizeof(g->vertexNames));
}

void addEdge(Graph *g, int u, int v) {
    if (u >= 0 && u < g->n && v >= 0 && v < g->n) {
        if (g->adj[u][v] == 0) {
            g->adj[u][v] = 1;
            g->adj[v][u] = 1;
            g->degree[u]++;
            g->degree[v]++;
        }
    }
}

int readGraphFromFile(Graph *g, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Ошибка: не удалось открыть файл %s\n", filename);
        return 0;
    }

    int n;
    if (fscanf(f, "%d", &n) != 1) {
        printf("Ошибка чтения количества вершин\n");
        fclose(f);
        return 0;
    }

    initGraph(g, n);

    char line[256];
    int edgeCount = 0;
    int isNumeric = -1;

    while (fgets(line, sizeof(line), f)) {
        if (strlen(line) <= 1) continue;

        char v1[20], v2[20];
        if (sscanf(line, "%s %s", v1, v2) == 2) {
            if (isNumeric == -1) {
                isNumeric = isdigit(v1[0]) ? 1 : 0;
            }

            int u, v;
            if (isNumeric) {
                u = atoi(v1) - 1;
                v = atoi(v2) - 1;
                sprintf(g->vertexNames[u], "%d", u + 1);
                sprintf(g->vertexNames[v], "%d", v + 1);
            } else {
                u = tolower(v1[0]) - 'a';
                v = tolower(v2[0]) - 'a';
                sprintf(g->vertexNames[u], "%c", v1[0]);
                sprintf(g->vertexNames[v], "%c", v2[0]);
            }

            if (u >= 0 && u < n && v >= 0 && v < n) {
                addEdge(g, u, v);
                edgeCount++;
            } else {
                printf("Предупреждение: вершина вне диапазона [%d, %d]\n", u, v);
            }
        }
    }

    fclose(f);
    printf("Прочитано %d вершин и %d рёбер из файла %s\n", n, edgeCount, filename);
    return 1;
}

int isConnected(Graph *g) {
    if (g->n == 0) return 0;

    int visited[MAX_V] = {0};
    int queue[MAX_V], front = 0, rear = 0;
    int count = 0;

    queue[rear++] = 0;
    visited[0] = 1;

    while (front < rear) {
        int v = queue[front++];
        count++;
        for (int i = 0; i < g->n; i++) {
            if (g->adj[v][i] && !visited[i]) {
                visited[i] = 1;
                queue[rear++] = i;
            }
        }
    }
    return count == g->n;
}

int hasEulerianCycle(Graph *g) {
    if (!isConnected(g)) {
        printf("Граф несвязен -> эйлерова цикла нет\n");
        return 0;
    }
    for (int i = 0; i < g->n; i++) {
        if (g->degree[i] % 2 != 0) {
            printf("Вершина %s имеет нечётную степень %d\n", 
                   g->vertexNames[i], g->degree[i]);
            return 0;
        }
    }
    return 1;
}

void findEulerianCycle(Graph *g) {
    printf("\n--- Проверка эйлерова цикла ---\n");
    if (!hasEulerianCycle(g)) {
        printf("Эйлеров цикл не существует в данном графе.\n");
        return;
    }

    // Копия матрицы смежности
    int adjCopy[MAX_V][MAX_V];
    memcpy(adjCopy, g->adj, sizeof(g->adj));

    int stack[MAX_E * 2];
    int cycle[MAX_E * 2];
    int stackTop = 0, cycleTop = 0;

    stack[stackTop++] = 0;

    while (stackTop > 0) {
        int v = stack[stackTop - 1];
        int found = 0;

        for (int i = 0; i < g->n; i++) {
            if (adjCopy[v][i]) {
                adjCopy[v][i] = 0;
                adjCopy[i][v] = 0;
                stack[stackTop++] = i;
                found = 1;
                break;
            }
        }

        if (!found) {
            cycle[cycleTop++] = stack[--stackTop];
        }
    }

    printf("Эйлеров цикл (последовательность вершин):\n");
    for (int i = cycleTop - 1; i >= 0; i--) {
        printf("%s", g->vertexNames[cycle[i]]);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
    printf("Длина цикла (число рёбер): %d\n", cycleTop - 1);
}

int hamPath[MAX_V];
int hamVisited[MAX_V];
int hamFound;

void hamiltonBacktrack(Graph *g, int pos, int start) {
    if (hamFound) return;

    if (pos == g->n) {
        if (g->adj[hamPath[pos - 1]][start]) {
            hamFound = 1;
        }
        return;
    }

    int last = (pos == 0) ? start : hamPath[pos - 1];

    for (int i = 0; i < g->n; i++) {
        if (!hamVisited[i] && g->adj[last][i]) {
            hamVisited[i] = 1;
            hamPath[pos] = i;
            hamiltonBacktrack(g, pos + 1, start);
            if (hamFound) return;
            hamVisited[i] = 0;
        }
    }
}

void findHamiltonianCycle(Graph *g) {
    printf("\n--- Поиск гамильтонова цикла ---\n");
    hamFound = 0;
    memset(hamVisited, 0, sizeof(hamVisited));
    memset(hamPath, 0, sizeof(hamPath));

    for (int start = 0; start < g->n && !hamFound; start++) {
        hamVisited[start] = 1;
        hamPath[0] = start;
        hamiltonBacktrack(g, 1, start);
        if (!hamFound) {
            hamVisited[start] = 0;
        }
    }

    if (hamFound) {
        printf("Гамильтонов цикл (последовательность вершин):\n");
        for (int i = 0; i < g->n; i++) {
            printf("%s", g->vertexNames[hamPath[i]]);
            if (i < g->n - 1) printf(" -> ");
        }
        printf(" -> %s\n", g->vertexNames[hamPath[0]]);
    } else {
        printf("Гамильтонов цикл не существует в данном графе.\n");
    }
}

void printGraphInfo(Graph *g) {
    printf("\nСтепени вершин:\n");
    for (int i = 0; i < g->n; i++) {
        printf("  %s: %d\n", g->vertexNames[i], g->degree[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Использование: %s <файл_графа.txt>\n", argv[0]);
        printf("Пример: %s graph1.txt\n", argv[0]);
        return 1;
    }

    Graph g;
    const char *filename = argv[1];

    printf("============================================\n");
    printf("  Чтение графа из файла: %s\n", filename);
    printf("============================================\n");

    if (!readGraphFromFile(&g, filename)) {
        return 1;
    }

    printGraphInfo(&g);

    findEulerianCycle(&g);

    findHamiltonianCycle(&g);

    return 0;
}
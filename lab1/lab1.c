#pragma execution_character_set("utf-8")
#include <stdio.h>
#include <locale.h>

#define MAX_NODES 100 

int adj[MAX_NODES][MAX_NODES] = {0};

// Функция добавления ребра в неориентированный граф
void addEdge(int u, int v) {
    if (u >= 1 && u < MAX_NODES && v >= 1 && v < MAX_NODES) {
        adj[u][v] = 1;
        adj[v][u] = 1;
    } else {
        printf("Предупреждение: вершины %d и %d выходят за допустимый диапазон (1..%d)\n", u, v, MAX_NODES - 1);
    }
}


//Поиск в глубину (DFS)
void DFS(int u, int visited[], int n) {
    visited[u] = 1; 
    for (int v = 1; v <= n; v++) {
        if (adj[u][v] == 1 && !visited[v]) {
            DFS(v, visited, n); 
        }
    }
}

// Подсчет компонент связности с помощью DFS
int countComponentsDFS(int n) {
    int visited[MAX_NODES] = {0};
    int count = 0;

    for (int i = 1; i <= n; i++) {
        if (!visited[i]) {
            DFS(i, visited, n);
            count++;          
        }
    }
    return count;
}


//Поиск в ширину (BFS)
void BFS(int start, int visited[], int n) {
    int queue[MAX_NODES];
    int front = 0, rear = 0;

    visited[start] = 1;
    queue[rear++] = start;

    while (front < rear) {
        int u = queue[front++];
        
        for (int v = 1; v <= n; v++) {
            if (adj[u][v] == 1 && !visited[v]) {
                visited[v] = 1;        
                queue[rear++] = v;    
            }
        }
    }
}

// Подсчет компонент связности с помощью BFS
int countComponentsBFS(int n) {
    int visited[MAX_NODES] = {0};
    int count = 0;

    for (int i = 1; i <= n; i++) {
        if (!visited[i]) {
            BFS(i, visited, n); 
            count++;   
        }
    }
    return count;
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int n, m;

    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            adj[i][j] = 0;
        }
    }

    printf("Введите количество вершин в графе (n, от 1 до %d): ", MAX_NODES - 1);
    if (scanf("%d", &n) != 1 || n < 1 || n >= MAX_NODES) {
        printf("Некорректное количество вершин. Завершение программы.\n");
        return 1;
    }

    printf("Введите количество ребер в графе (m): ");
    if (scanf("%d", &m) != 1 || m < 0) {
        printf("Некорректное количество ребер. Завершение программы.\n");
        return 1;
    }

    printf("Введите %d ребер в формате 'u v' (вершины нумеруются от 1 до %d):\n", m, n);
    for (int i = 0; i < m; i++) {
        int u, v;
        scanf("%d %d", &u, &v);
        addEdge(u, v);
    }

    int dfs_count = countComponentsDFS(n);
    int bfs_count = countComponentsBFS(n);
    
    printf("\n--- Результаты ---\n");
    printf("Число компонент связности (метод DFS): %d\n", dfs_count);
    printf("Число компонент связности (метод BFS): %d\n", bfs_count);

    return 0;
}
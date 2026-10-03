/*
 * ALUMNO(S):
 * Angulo Diaz Julio Abraham
 *
 * ACTIVIDAD: Actividad 1.3 - Reduce en MPI con OpenMP
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <limits>
#include <iomanip>
#include <windows.h> // Para sincronizacion de red
#include <mpi.h>
#include <omp.h>

using namespace std;

// ==========================
// Clase OperacionesArreglos
// ==========================
class OperacionesArreglos {
public:
    void llenarSecuencialMPI(int *A, int n, char *nombrePC, int nodo, int offset, int detallado, int size) {
        if (!detallado) {
            int i;
            #pragma omp parallel for private(i)
            for (i = 0; i < n; i++) {
                A[i] = (offset + i) + 1;
            }
        } else {
            // Impresion ordenada por turno de nodo para evitar desfase de red
            for (int r = 0; r < size; r++) {
                if (nodo == r) {
                    int i;
                    #pragma omp parallel for private(i)
                    for (i = 0; i < n; i++) {
                        int hilo = omp_get_thread_num();
                        A[i] = (offset + i) + 1;
                        #pragma omp critical
                        {
                            printf("PC:%s Nodo:%d Hilo:%d A[%d]=%d\n", nombrePC, nodo, hilo, offset + i, A[i]);
                            fflush(stdout);
                        }
                    }
                    fflush(stdout);
                    Sleep(150); // Pausa para vaciar buffer TCP hacia el master
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
    }

    void llenarAleatorioMPI(int *A, int n, char *nombrePC, int nodo, int offset, int max_val, int detallado, int size) {
        if (!detallado) {
            int i;
            #pragma omp parallel for private(i)
            for (i = 0; i < n; i++) {
                A[i] = (rand() % max_val) + 1;
            }
        } else {
            for (int r = 0; r < size; r++) {
                if (nodo == r) {
                    int i;
                    #pragma omp parallel for private(i)
                    for (i = 0; i < n; i++) {
                        int hilo = omp_get_thread_num();
                        A[i] = (rand() % max_val) + 1;
                        #pragma omp critical
                        {
                            printf("PC:%s Nodo:%d Hilo:%d A[%d]=%d\n", nombrePC, nodo, hilo, offset + i, A[i]);
                            fflush(stdout);
                        }
                    }
                    fflush(stdout);
                    Sleep(150);
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
    }

    long long sumatoriaMPI(int *A, int n, char *nombrePC, int nodo, int offset, int detallado, int size) {
        long long suma = 0;
        if (!detallado) {
            int i;
            #pragma omp parallel for reduction(+:suma) private(i)
            for (i = 0; i < n; i++) {
                suma += A[i];
            }
        } else {
            for (int r = 0; r < size; r++) {
                if (nodo == r) {
                    int i;
                    #pragma omp parallel for reduction(+:suma) private(i)
                    for (i = 0; i < n; i++) {
                        int hilo = omp_get_thread_num();
                        suma += A[i];
                        #pragma omp critical
                        {
                            printf("PC:%s Nodo:%d Hilo:%d suma A[%d]=%d\n", nombrePC, nodo, hilo, offset + i, A[i]);
                            fflush(stdout);
                        }
                    }
                    fflush(stdout);
                    Sleep(150);
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
        return suma;
    }

    double promedioMPI(int *A, int n, char *nombrePC, int nodo, int offset, int detallado, int size) {
        long long suma = sumatoriaMPI(A, n, nombrePC, nodo, offset, detallado, size);
        return (double)suma / n;
    }

    int maximoMPI(int *A, int n, char *nombrePC, int nodo, int offset, int detallado, int size) {
        int max_val = (n > 0) ? A[0] : INT_MIN;
        if (!detallado) {
            int i;
            #pragma omp parallel for reduction(max:max_val) private(i)
            for (i = 0; i < n; i++) {
                if (A[i] > max_val) max_val = A[i];
            }
        } else {
            for (int r = 0; r < size; r++) {
                if (nodo == r) {
                    int i;
                    #pragma omp parallel for reduction(max:max_val) private(i)
                    for (i = 0; i < n; i++) {
                        int hilo = omp_get_thread_num();
                        if (A[i] > max_val) max_val = A[i];
                        #pragma omp critical
                        {
                            printf("PC:%s Nodo:%d Hilo:%d revisa A[%d]=%d\n", nombrePC, nodo, hilo, offset + i, A[i]);
                            fflush(stdout);
                        }
                    }
                    fflush(stdout);
                    Sleep(150);
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
        return max_val;
    }

    int minimoMPI(int *A, int n, char *nombrePC, int nodo, int offset, int detallado, int size) {
        int min_val = (n > 0) ? A[0] : INT_MAX;
        if (!detallado) {
            int i;
            #pragma omp parallel for reduction(min:min_val) private(i)
            for (i = 0; i < n; i++) {
                if (A[i] < min_val) min_val = A[i];
            }
        } else {
            for (int r = 0; r < size; r++) {
                if (nodo == r) {
                    int i;
                    #pragma omp parallel for reduction(min:min_val) private(i)
                    for (i = 0; i < n; i++) {
                        int hilo = omp_get_thread_num();
                        if (A[i] < min_val) min_val = A[i];
                        #pragma omp critical
                        {
                            printf("PC:%s Nodo:%d Hilo:%d revisa A[%d]=%d\n", nombrePC, nodo, hilo, offset + i, A[i]);
                            fflush(stdout);
                        }
                    }
                    fflush(stdout);
                    Sleep(150);
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
        return min_val;
    }
};

void limpiarTeclado() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main(int argc, char* argv[]) {
    int proporcionado;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &proporcionado);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char nombrePC[MPI_MAX_PROCESSOR_NAME];
    int longitud_nombre;
    MPI_Get_processor_name(nombrePC, &longitud_nombre);

    if (rank == 0) {
        cout << "==========================================" << endl;
        cout << "ALUMNO: Angulo Diaz Julio Abraham" << endl;
        cout << "==========================================" << endl;
    }

    OperacionesArreglos op;
    int *A_local = NULL;
    int n_total = 0;
    int n_local = 0;
    int offset = 0;
    int detallado = 0;
    int opcion = -1;

    do {
        if (rank == 0) {
            cout << "\n==========================================" << endl;
            cout << "   MENU - Actividad 1.3: Reduce en MPI con OpenMP   " << endl;
            cout << "==========================================" << endl;
            cout << "1. Crear arreglos (Configurar N)" << endl;
            cout << "6. Llenar secuencial" << endl;
            cout << "7. Llenar aleatorio" << endl;
            cout << "8. Sumatoria" << endl;
            cout << "9. Promedio" << endl;
            cout << "10. Maximo" << endl;
            cout << "11. Minimo" << endl;
            cout << "0. Salir del programa" << endl;
            cout << "Seleccione una opcion: " << flush;

            if (!(cin >> opcion)) {
                limpiarTeclado();
                opcion = -1;
            } else {
                limpiarTeclado();
            }
        }

        MPI_Bcast(&opcion, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (opcion == 0) break;

        if (opcion == 1) {
            if (rank == 0) {
                cout << "Ingrese el tamanio N del arreglo: " << flush;
                cin >> n_total;
                limpiarTeclado();

                cout << "Desea impresion detallada por elemento/hilo? (1: Si, 0: No): " << flush;
                cin >> detallado;
                limpiarTeclado();
            }

            MPI_Bcast(&n_total, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&detallado, 1, MPI_INT, 0, MPI_COMM_WORLD);

            int base = n_total / size;
            int resto = n_total % size;
            n_local = base + (rank < resto ? 1 : 0);
            offset = rank * base + (rank < resto ? rank : resto);

            if (A_local != NULL) delete[] A_local;
            A_local = new int[n_local];

            if (rank == 0) {
                cout << ">> Arreglo configurado a N = " << n_total
                     << " | Modo detallado = " << (detallado ? "SI" : "NO") << endl;
            }
        }
        else if (opcion == 6 || opcion == 7) {
            if (n_total <= 0 || A_local == NULL) {
                if (rank == 0) cout << ">> Error: Primero cree el arreglo (Opcion 1)." << endl;
                continue;
            }

            srand(time(NULL) + rank);

            if (opcion == 6) {
                op.llenarSecuencialMPI(A_local, n_local, nombrePC, rank, offset, detallado, size);
                if (rank == 0) cout << ">> Arreglo llenado de forma secuencial." << endl;
            } else {
                int max_val = (n_total > 1000) ? 1000000 : 1000;
                op.llenarAleatorioMPI(A_local, n_local, nombrePC, rank, offset, max_val, detallado, size);
                if (rank == 0) cout << ">> Arreglo llenado de forma aleatoria." << endl;
            }
        }
        else if (opcion >= 8 && opcion <= 11) {
            if (n_total <= 0 || A_local == NULL) {
                if (rank == 0) cout << ">> Error: Primero cree y llene el arreglo." << endl;
                continue;
            }

            double t_inicio = MPI_Wtime();

            if (opcion == 8) { // Sumatoria
                long long suma_local = op.sumatoriaMPI(A_local, n_local, nombrePC, rank, offset, detallado, size);
                long long suma_total = 0;
                MPI_Reduce(&suma_local, &suma_total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
                double t_fin = MPI_Wtime();

                if (rank == 0) {
                    cout << ">> SUMATORIA TOTAL: " << suma_total << endl;
                    cout << ">> Tiempo de ejecucion: " << (t_fin - t_inicio) << " segundos." << endl;
                }
            }
            else if (opcion == 9) { // Promedio
                long long suma_local = op.sumatoriaMPI(A_local, n_local, nombrePC, rank, offset, detallado, size);
                long long suma_total = 0;
                MPI_Reduce(&suma_local, &suma_total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
                double t_fin = MPI_Wtime();

                if (rank == 0) {
                    cout << ">> PROMEDIO: " << (double)suma_total / n_total << endl;
                    cout << ">> Tiempo de ejecucion: " << (t_fin - t_inicio) << " segundos." << endl;
                }
            }
            else if (opcion == 10) { // Maximo
                int max_local = op.maximoMPI(A_local, n_local, nombrePC, rank, offset, detallado, size);
                int max_global = INT_MIN;
                MPI_Reduce(&max_local, &max_global, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
                double t_fin = MPI_Wtime();

                if (rank == 0) {
                    cout << ">> MAXIMO VALOR: " << max_global << endl;
                    cout << ">> Tiempo de ejecucion: " << (t_fin - t_inicio) << " segundos." << endl;
                }
            }
            else if (opcion == 11) { // Minimo
                int min_local = op.minimoMPI(A_local, n_local, nombrePC, rank, offset, detallado, size);
                int min_global = INT_MAX;
                MPI_Reduce(&min_local, &min_global, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
                double t_fin = MPI_Wtime();

                if (rank == 0) {
                    cout << ">> MINIMO VALOR: " << min_global << endl;
                    cout << ">> Tiempo de ejecucion: " << (t_fin - t_inicio) << " segundos." << endl;
                }
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

    } while (opcion != 0);

    if (A_local != NULL) {
        delete[] A_local;
    }

    if (rank == 0) {
        cout << "==========================================" << endl;
        cout << "ALUMNO: Angulo Diaz Julio Abraham" << endl;
        cout << "==========================================" << endl;
    }

    MPI_Finalize();
    return 0;
}

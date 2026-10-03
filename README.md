# Actividad-1.3-MPI-OpenMP
Actividad 1.3: Reduce en MPI con OpenMP

**Alumno:** Angulo Diaz Julio Abraham  
**Materia:** Programación Paralela y Distribuida  

## Descripción
Proyecto de computación paralela e híbrida (MPI + OpenMP) que implementa operaciones de reducción colectiva (`MPI_Reduce`) sobre arreglos distribuidos en un clúster heterogéneo de 3 máquinas físicas/virtuales y 5 procesos MPI.

## Arquitectura del Clúster
- **Nodo Máster (1 proceso):** Laptop Máster (Nodo 0)
- **Nodo Esclavo 1 (2 procesos):** Máquina Virtual 1 (`192.168.1.57` - Nodos 1 y 2)
- **Nodo Esclavo 2 (2 procesos):** Máquina Virtual 2 (`192.168.1.56` - Nodos 3 y 4)

## Ejecución en Clúster
```cmd
mpiexec -hosts 3 localhost 1 192.168.1.57 2 192.168.1.56 2 C:\practica\practica1.3.exe

#include <iostream>
#include <string>
#include <limits> // Para limpiar el buffer de entrada
#include <cstdlib> // <stdlib.h> es de C, <cstdlib> es de C++

using namespace std;

// --- ESTRUCTURAS DE DATOS ---

// Estructura para el Gestor de Procesos (Lista Enlazada)
struct Proceso {
    int pid;
    string nombre;
    int prioridad;
    Proceso* siguiente; // Puntero al siguiente proceso en la lista
};

// Estructura para el Gestor de Memoria (Pila)
struct BloqueMemoria {
    Proceso* proceso; // Proceso asociado a este bloque de memoria
    int tamanio;
    BloqueMemoria* siguiente; // Puntero al siguiente bloque en la pila
};

// Estructura para el Planificador de CPU (Cola de Prioridad)
struct NodoCola {
    Proceso* proceso; // Apuntador al proceso asociado
    NodoCola* siguiente; // Apuntador al siguiente nodo en la cola
};

// --- PUNTEROS GLOBALES (CABEZAS DE LAS ESTRUCTURAS) ---

Proceso* cabezaProcesos = NULL; // Puntero al inicio de la lista de procesos
BloqueMemoria* topeMemoria = NULL; // Puntero al tope de la pila de memoria
NodoCola* cabezaCola = NULL; // Puntero a la cabeza de la cola del planificador

// --- FUNCIONES AUXILIARES ---

// Busca un proceso por PID en la lista enlazada
Proceso* buscarProcesoPorPID(int pid) {
    Proceso* actual = cabezaProcesos;
    while (actual != NULL) {
        if (actual->pid == pid) {
            return actual; // Retorna el proceso si lo encuentra
        }
        actual = actual->siguiente;
    }
    return NULL; // Retorna NULL si no se encuentra
}

// Verifica si un proceso ya está en la cola del planificador
bool estaEnCola(int pid) {
    NodoCola* actual = cabezaCola;
    while (actual != NULL) {
        if (actual->proceso->pid == pid) {
            return true; // Retorna true si el PID ya está en la cola
        }
        actual = actual->siguiente;
    }
    return false; // Retorna false si no está
}

// Limpia el buffer de entrada
void limpiarBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Limpia la pantalla y espera al usuario (VERSIÓN CORREGIDA)
void limpiarYPausar() {
    cout << "Presione Enter para continuar...";
    // 1. Limpia cualquier \n que haya quedado del 'cin >>' anterior
    limpiarBuffer();
    // 2. Espera SIEMPRE un nuevo Enter.
    // El 'if (cin.gcount() == 0)' original era incorrecto.
    cin.get();
    system("cls || clear"); // "cls" para Windows, "clear" para Linux/Mac
}


// --- (INICIO DE LA CORRECCIÓN) FUNCIONES AUXILIARES PARA BORRADO SEGURO ---

/**
 * (NUEVO) Elimina todas las entradas de un PID de la Pila de Memoria.
 * Esto es necesario para evitar punteros colgantes cuando se borra un Proceso.
 */
void eliminarProcesosDePila(int pid) {
    if (topeMemoria == NULL) return;

    BloqueMemoria* actual = topeMemoria;
    BloqueMemoria* prev = NULL;

    // Bucle para eliminar todos los bloques que coincidan en el TOPE
    while (actual != NULL && actual->proceso->pid == pid) {
        BloqueMemoria* temp = actual;
        actual = actual->siguiente;
        topeMemoria = actual; // Mover el tope
        delete temp;
        cout << "  -> Bloque de memoria (PID: " << pid << ") liberado de la Pila.\n";
    }

    // Si la pila se vació, salir
    if (actual == NULL) return;

    // Recorrer el resto de la pila
    prev = actual;
    actual = actual->siguiente;

    while (actual != NULL) {
        if (actual->proceso->pid == pid) {
            // Eliminar 'actual'
            BloqueMemoria* temp = actual;
            prev->siguiente = actual->siguiente; // Enlazar el anterior con el siguiente
            actual = actual->siguiente; // Mover 'actual'
            delete temp;
            cout << "  -> Bloque de memoria (PID: " << pid << ") liberado de la Pila.\n";
        } else {
            // Avanzar ambos
            prev = actual;
            actual = actual->siguiente;
        }
    }
}

/**
 * (NUEVO) Elimina la entrada de un PID de la Cola del Planificador.
 */
void eliminarProcesoDeCola(int pid) {
    if (cabezaCola == NULL) return;

    NodoCola* temp = NULL;
    // Caso 1: El nodo a eliminar es la cabeza
    if (cabezaCola->proceso->pid == pid) {
        temp = cabezaCola;
        cabezaCola = cabezaCola->siguiente;
        delete temp;
        cout << "  -> Proceso (PID: " << pid << ") eliminado de la Cola de CPU.\n";
        return; // Un proceso solo puede estar una vez en la cola
    }
    
    // Caso 2: El nodo está en otra parte
    NodoCola* actual = cabezaCola;
    while (actual->siguiente != NULL && actual->siguiente->proceso->pid != pid) {
        actual = actual->siguiente;
    }

    // Si se encontró
    if (actual->siguiente != NULL) {
        temp = actual->siguiente;
        actual->siguiente = temp->siguiente;
        delete temp;
        cout << "  -> Proceso (PID: " << pid << ") eliminado de la Cola de CPU.\n";
    }
}

// --- (FIN DE LA CORRECCIÓN) ---


// --- GESTOR DE PROCESOS (LISTA ENLAZADA) ---

// 1.1 Insertar nuevo proceso
void insertarProceso() {
    int pid, prioridad;
    string nombre;

    // Solicitar y validar PID
    do {
        cout << "Ingrese PID (entero positivo): ";
        while (!(cin >> pid) || pid <= 0) {
            cout << "Error: El PID debe ser un numero entero positivo.\n";
            limpiarBuffer();
            cout << "Ingrese PID (entero positivo): ";
        }
        if (buscarProcesoPorPID(pid) != NULL) {
            cout << "Error: Ya existe un proceso con ese PID.\n";
            pid = 0; // Forzar a repetir el bucle
        }
    } while (pid <= 0);

    limpiarBuffer(); // Limpiar buffer después de cin >> pid

    // Solicitar y validar Nombre
    do {
        cout << "Ingrese nombre del proceso: ";
        getline(cin, nombre);
        if (nombre.empty()) {
            cout << "Error: la cadena no puede estar vacia.\n";
        }
    } while (nombre.empty());

    // Solicitar y validar Prioridad
    do {
        cout << "Ingrese prioridad (entero positivo, 1 es mas prioritario): ";
        while (!(cin >> prioridad) || prioridad <= 0) {
            cout << "Error: La prioridad debe ser un numero entero positivo.\n";
            limpiarBuffer();
            cout << "Ingrese prioridad (entero positivo): ";
        }
    } while (prioridad <= 0);

    // Crear el nuevo proceso
    Proceso* nuevo = new Proceso();
    nuevo->pid = pid;
    nuevo->nombre = nombre;
    nuevo->prioridad = prioridad;
    nuevo->siguiente = NULL;

    // Insertar en la lista
    if (cabezaProcesos == NULL) {
        cabezaProcesos = nuevo; // Si la lista está vacía
    } else {
        Proceso* actual = cabezaProcesos;
        while (actual->siguiente != NULL) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo; // Añadir al final
    }

    cout << "Proceso insertado correctamente.\n";
    limpiarYPausar();
}

// 1.2 Eliminar proceso (VERSIÓN CORREGIDA)
void eliminarProceso() {
    int pid;
    cout << "Ingrese PID del proceso a eliminar: ";
    if (!(cin >> pid) || pid <= 0) {
        cout << "PID invalido.\n";
        limpiarBuffer();
        limpiarYPausar();
        return;
    }

    if (cabezaProcesos == NULL) {
        cout << "No hay procesos para eliminar.\n";
        limpiarYPausar();
        return;
    }

    Proceso* aEliminar = NULL;

    // 1. Buscar y desenlazar de la lista principal
    // Caso 1: El nodo a eliminar es la cabeza
    if (cabezaProcesos->pid == pid) {
        aEliminar = cabezaProcesos;
        cabezaProcesos = cabezaProcesos->siguiente;
    } else {
        // Caso 2: El nodo está en otra parte
        Proceso* actual = cabezaProcesos;
        while (actual->siguiente != NULL && actual->siguiente->pid != pid) {
            actual = actual->siguiente;
        }
        if (actual->siguiente != NULL) {
            aEliminar = actual->siguiente;
            actual->siguiente = aEliminar->siguiente;
        }
    }

    // 2. Si se encontró (aEliminar != NULL), eliminarlo de las otras estructuras
    if (aEliminar != NULL) {
        cout << "Proceso (PID: " << pid << ") encontrado. Eliminando de todas las estructuras...\n";
        
        // --- INICIO DE LA CORRECCIÓN ---
        // 2.1 Eliminar de la Pila de Memoria
        eliminarProcesosDePila(pid);
        
        // 2.2 Eliminar de la Cola de CPU
        eliminarProcesoDeCola(pid);
        // --- FIN DE LA CORRECCIÓN ---

        delete aEliminar; // Libera la memoria
        cout << "Proceso con PID " << pid << " eliminado completamente.\n";

    } else {
        cout << "Proceso con PID " << pid << " no encontrado.\n";
    }
    limpiarYPausar();
}

// 1.3 Mostrar todos los procesos
void mostrarProcesos() {
    cout << "\n--- Lista de Todos los Procesos ---\n";
    if (cabezaProcesos == NULL) {
        cout << "No hay procesos registrados.\n";
    } else {
        Proceso* actual = cabezaProcesos;
        while (actual != NULL) {
            cout << "PID: " << actual->pid 
                 << ", Nombre: " << actual->nombre 
                 << ", Prioridad: " << actual->prioridad << "\n";
            actual = actual->siguiente;
        }
    }
    limpiarYPausar();
}

// --- PLANIFICADOR DE CPU (COLA DE PRIORIDAD) ---

// 2.1 Encolar proceso en el planificador
void encolarProcesoEnPlanificador() {
    int pid;
    cout << "Ingrese PID del proceso a encolar: ";
    if (!(cin >> pid) || pid <= 0) {
        cout << "PID invalido.\n";
        limpiarBuffer();
        limpiarYPausar();
        return;
    }

    Proceso* p = buscarProcesoPorPID(pid);
    if (p == NULL) {
        cout << "Error: Proceso con PID " << pid << " no existe en la lista general.\n";
        limpiarYPausar();
        return;
    }

    if (estaEnCola(pid)) {
        cout << "Error: El proceso ya esta en la cola del planificador.\n";
        limpiarYPausar();
        return;
    }

    // Crear nuevo nodo para la cola
    NodoCola* nuevo = new NodoCola();
    nuevo->proceso = p;
    nuevo->siguiente = NULL;

    // Insertar en la cola por prioridad (menor número = mayor prioridad)
    if (cabezaCola == NULL || p->prioridad < cabezaCola->proceso->prioridad) {
        // Insertar al inicio
        nuevo->siguiente = cabezaCola;
        cabezaCola = nuevo;
    } else {
        // Buscar posición
        NodoCola* actual = cabezaCola;
        while (actual->siguiente != NULL && actual->siguiente->proceso->prioridad <= p->prioridad) {
            actual = actual->siguiente;
        }
        nuevo->siguiente = actual->siguiente;
        actual->siguiente = nuevo;
    }

    cout << "Proceso " << p->nombre << " (PID: " << p->pid << ") encolado.\n";
    limpiarYPausar();
}

// 2.2 Desencolar y ejecutar proceso
void desencolaryEjecutarProceso() {
    if (cabezaCola == NULL) {
        cout << "La cola del planificador esta vacia. No hay procesos que ejecutar.\n";
        limpiarYPausar();
        return;
    }

    NodoCola* temp = cabezaCola; // Guardar el nodo a desencolar
    Proceso* p = temp->proceso;
    
    cabezaCola = cabezaCola->siguiente; // Mover la cabeza al siguiente

    cout << "Ejecutando proceso (Mayor Prioridad):\n";
    cout << "PID: " << p->pid << ", Nombre: " << p->nombre << ", Prioridad: " << p->prioridad << "\n";
    
    delete temp; // Liberar memoria del nodo de la cola
    limpiarYPausar();
}

// 2.3 Mostrar cola actual
void mostrarColaPlanificador() {
    cout << "\n--- Cola de Planificacion (Orden de Ejecucion) ---\n";
    if (cabezaCola == NULL) {
        cout << "La cola esta vacia.\n";
    } else {
        NodoCola* actual = cabezaCola;
        int i = 1;
        while (actual != NULL) {
            cout << i++ << ". PID: " << actual->proceso->pid 
                 << ", Nombre: " << actual->proceso->nombre 
                 << ", Prioridad: " << actual->proceso->prioridad << "\n";
            actual = actual->siguiente;
        }
    }
    limpiarYPausar();
}

// --- GESTOR DE MEMORIA (PILA) ---

// 3.1 Asignar memoria (Push)
void asignarMemoria() {
    int pid, tamanio;
    
    cout << "Ingrese PID del proceso al que se asignara memoria: ";
    if (!(cin >> pid) || pid <= 0) {
        cout << "PID invalido.\n";
        limpiarBuffer();
        limpiarYPausar();
        return;
    }

    Proceso* p = buscarProcesoPorPID(pid);
    if (p == NULL) {
        cout << "Error: Proceso con PID " << pid << " no existe.\n";
        limpiarYPausar();
        return;
    }

    cout << "Ingrese tamano de memoria a asignar (KB): ";
    if (!(cin >> tamanio) || tamanio <= 0) {
        cout << "Tamano invalido. Debe ser entero positivo.\n";
        limpiarBuffer();
        limpiarYPausar();
        return;
    }

    // Crear nuevo bloque de memoria (Push)
    BloqueMemoria* nuevo = new BloqueMemoria();
    nuevo->proceso = p;
    nuevo->tamanio = tamanio;
    nuevo->siguiente = topeMemoria; // Enlaza al bloque anterior
    topeMemoria = nuevo; // El nuevo bloque es ahora el tope

    cout << "Memoria asignada al proceso " << p->nombre << " (PID: " << p->pid << ").\n";
    limpiarYPausar();
}

// 3.2 Liberar memoria (Pop)
void liberarMemoria() {
    if (topeMemoria == NULL) {
        cout << "La pila de memoria esta vacia. No hay nada que liberar.\n";
        limpiarYPausar();
        return;
    }

    BloqueMemoria* temp = topeMemoria; // Guardar el bloque superior
    topeMemoria = topeMemoria->siguiente; // Mover el tope al siguiente

    cout << "Memoria liberada del proceso: " << temp->proceso->nombre 
         << " (PID: " << temp->proceso->pid << ", Tamano: " << temp->tamanio << "KB)\n";
    
    delete temp; // Liberar el bloque de memoria
    limpiarYPausar();
}

// 3.3 Ver estado actual de la memoria (Recorrer Pila)
void estadoMemoria() {
    cout << "\n--- Estado Actual de la Pila de Memoria ---\n";
    if (topeMemoria == NULL) {
        cout << "Pila de memoria vacia. No hay memoria asignada.\n";
    } else {
        cout << "(Tope)\n";
        BloqueMemoria* actual = topeMemoria;
        while (actual != NULL) {
            cout << "  Proceso: " << actual->proceso->nombre << " (PID: " << actual->proceso->pid << ")\n"
                 << "  Tamano: " << actual->tamanio << " KB\n"
                 << "  ||\n"
                 << "  \\/\n";
            actual = actual->siguiente;
        }
        cout << "(Base)\n";
    }
    limpiarYPausar();
}


// --- MENÚ PRINCIPAL ---

void menuGestorProcesos() {
    int opcion;
    do {
        system("cls || clear");
        cout << "\n--- Gestor de Procesos (Lista Enlazada) ---\n";
        cout << "1. Insertar nuevo proceso\n";
        cout << "2. Eliminar proceso\n";
        cout << "3. Mostrar todos los procesos\n";
        cout << "4. Volver al menu principal\n";
        cout << "Seleccione una opcion (1-4): ";
        
        if (!(cin >> opcion)) {
            cout << "Opcion invalida.\n";
            limpiarBuffer();
            opcion = 0; // Para repetir el bucle
        }

        switch (opcion) {
            case 1: insertarProceso(); break;
            case 2: eliminarProceso(); break;
            case 3: mostrarProcesos(); break;
            case 4: cout << "Volviendo al menu principal...\n"; break;
            default: cout << "Opcion invalida.\n"; limpiarYPausar(); break;
        }
    } while (opcion != 4);
}

void menuPlanificadorCPU() {
    int opcion;
    do {
        system("cls || clear");
        cout << "\n--- Planificador de CPU (Cola de Prioridad) ---\n";
        cout << "1. Encolar proceso por PID\n";
        cout << "2. Desencolar y ejecutar proceso (Mayor Prioridad)\n";
        cout << "3. Mostrar cola actual\n";
        cout << "4. Volver al menu principal\n";
        cout << "Seleccione una opcion (1-4): ";
        
        if (!(cin >> opcion)) {
            cout << "Opcion invalida.\n";
            limpiarBuffer();
            opcion = 0;
        }

        switch (opcion) {
            case 1: encolarProcesoEnPlanificador(); break;
            case 2: desencolaryEjecutarProceso(); break;
            case 3: mostrarColaPlanificador(); break;
            case 4: cout << "Volviendo al menu principal...\n"; break;
            default: cout << "Opcion invalida.\n"; limpiarYPausar(); break;
        }
    } while (opcion != 4);
}

void menuGestorMemoria() {
    int opcion;
    do {
        system("cls || clear");
        cout << "\n--- Gestor de Memoria (Pila) ---\n";
        cout << "1. Asignar memoria a proceso (Push)\n";
        cout << "2. Liberar memoria (Pop)\n";
        cout << "3. Ver estado actual de la memoria\n";
        cout << "4. Volver al menu principal\n";
        cout << "Seleccione una opcion (1-4): ";
        
        if (!(cin >> opcion)) {
            cout << "Opcion invalida.\n";
            limpiarBuffer();
            opcion = 0;
        }

        switch (opcion) {
            case 1: asignarMemoria(); break;
            case 2: liberarMemoria(); break;
            case 3: estadoMemoria(); break;
            case 4: cout << "Volviendo al menu principal...\n"; break;
            default: cout << "Opcion invalida.\n"; limpiarYPausar(); break;
        }
    } while (opcion != 4);
}

int main() {
    int opcionPrincipal;
    do {
        system("cls || clear"); // Limpia la pantalla
        cout << "\n=== Sistema de Gestion de Procesos ===\n";
        cout << "1. Gestor de Procesos (Listas)\n";
        cout << "2. Planificador de CPU (Colas)\n";
        cout << "3. Gestor de Memoria (Pilas)\n";
        cout << "4. Salir\n";
        cout << "Seleccione una opcion (1-4): ";

        if (!(cin >> opcionPrincipal)) {
            cout << "Opcion invalida. Por favor ingrese un numero entre 1 y 4.\n";
            limpiarBuffer(); // Descarta la entrada incorrecta
            opcionPrincipal = 0; // Asigna un valor inválido para repetir el bucle
            limpiarYPausar();
            continue;
        }

        switch (opcionPrincipal) {
            case 1:
                menuGestorProcesos();
                break;
            case 2:
                menuPlanificadorCPU();
                break;
            case 3:
                menuGestorMemoria();
                break;
            case 4:
                cout << "Saliendo del programa...\n";
                break;
            default:
                cout << "Opcion invalida. Por favor ingrese un numero entre 1 y 4.\n";
                limpiarYPausar();
                break;
        }
    } while (opcionPrincipal != 4);

    // --- Limpieza final de memoria (Buena práctica) ---
    // (Opcional para este ejercicio, pero importante en proyectos reales)
    // Liberar lista de procesos
    Proceso* procActual = cabezaProcesos;
    while (procActual != NULL) {
        Proceso* temp = procActual;
        procActual = procActual->siguiente;
        delete temp;
    }
    // Liberar pila de memoria
    BloqueMemoria* memActual = topeMemoria;
    while (memActual != NULL) {
        BloqueMemoria* temp = memActual;
        memActual = memActual->siguiente;
        delete temp;
    }
    // Liberar cola de CPU
    NodoCola* colaActual = cabezaCola;
    while (colaActual != NULL) {
        NodoCola* temp = colaActual;
        colaActual = colaActual->siguiente;
        delete temp;
    }

    return 0;
}

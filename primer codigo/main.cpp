#include <iostream>
#include <string>
#include <limits>
using namespace std;
// ESTRUCTURA DE UN PROCESO
struct Proceso {
    int id;
    string nombre;
    int prioridad;
    string estado;
    int tiempoCPU;
};

// ESTRUCTURA DE UN NODO (usa puntero al siguiente)
struct Nodo {
    Proceso *data;    // puntero al proceso (no copia)
    Nodo *sig;        // puntero al siguiente nodo

    Nodo(Proceso *p) {
        data = p;
        sig = NULL;
    }
};
// CLASE COLA (READY QUEUE)
struct Cola {
    Nodo *frente; // puntero al primer nodo
    Nodo *fin;    // puntero al último nodo

    Cola() {
        frente = NULL;
        fin = NULL;
    }

    bool vacia() {
        return frente == NULL;
    }

    void encolar(Proceso *p) {
        Nodo *nuevo = new Nodo(p);
        if (fin == NULL) {
            frente = fin = nuevo;
        } else {
            fin->sig = nuevo;
            fin = nuevo;
        }
    }

    bool desencolar(Proceso *&p) {
        if (vacia()) return false;
        Nodo *temp = frente;
        p = frente->data; // el puntero p apunta al proceso del nodo
        frente = frente->sig;
        if (frente == NULL) fin = NULL;
        delete temp; // liberamos el nodo, no el proceso
        return true;
    }

    void mostrar() {
        if (vacia()) {
            cout << "\n[Cola vacia]\n";
            return;
        }
        Nodo *aux = frente;
        cout << "\n--- COLA DE EJECUCION (READY QUEUE) ---\n";
        while (aux != NULL) {
            cout << "ID: " << aux->data->id
                 << " | Nombre: " << aux->data->nombre
                 << " | Prioridad: " << aux->data->prioridad
                 << " | Estado: " << aux->data->estado
                 << " | Tiempo CPU: " << aux->data->tiempoCPU << " ms\n";
            aux = aux->sig;
        }
    }
};
// CLASE PILA (PROCESOS FINALIZADOS)
struct Pila {
    Nodo *tope;

    Pila() {
        tope = NULL;
    }

    bool vacia() {
        return tope == NULL;
    }

    void push(Proceso *p) {
        Nodo *nuevo = new Nodo(p);
        nuevo->sig = tope;
        tope = nuevo;
    }

    bool pop(Proceso *&p) {
        if (vacia()) return false;
        Nodo *temp = tope;
        p = tope->data;
        tope = tope->sig;
        delete temp;
        return true;
    }

    void mostrar() {
        if (vacia()) {
            cout << "\n[Pila vacía]\n";
            return;
        }
        Nodo *aux = tope;
        cout << "\n--- PILA DE FINALIZADOS ---\n";
        while (aux != NULL) {
            cout << "ID: " << aux->data->id
                 << " | Nombre: " << aux->data->nombre
                 << " | Prioridad: " << aux->data->prioridad
                 << " | Estado: " << aux->data->estado
                 << " | Tiempo CPU: " << aux->data->tiempoCPU << " ms\n";
            aux = aux->sig;
        }
    }
};
// CLASE LISTA (TODOS LOS PROCESOS)
struct Lista {
    Nodo *inicio;

    Lista() {
        inicio = NULL;
    }

    void insertarFinal(Proceso *p) {
        Nodo *nuevo = new Nodo(p);
        if (inicio == NULL) {
            inicio = nuevo;
        } else {
            Nodo *aux = inicio;
            while (aux->sig != NULL) aux = aux->sig;
            aux->sig = nuevo;
        }
    }

    void mostrar() {
        if (inicio == NULL) {
            cout << "\n[Lista vacia]\n";
            return;
        }
        Nodo *aux = inicio;
        cout << "\n--- LISTA DE PROCESOS CREADOS ---\n";
        while (aux != NULL) {
            cout << "ID: " << aux->data->id
                 << " | Nombre: " << aux->data->nombre
                 << " | Prioridad: " << aux->data->prioridad
                 << " | Estado: " << aux->data->estado
                 << " | Tiempo CPU: " << aux->data->tiempoCPU << " ms\n";
            aux = aux->sig;
        }
    }
};
// FUNCIONES AUXILIARES
int generarID() {
    static int id = 1;
    return id++;
}

void pausa() {
    cout << "\nPresione ENTER para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void menu() {
    cout << "\n SISTEMA DE GESTION DE PROCESOS (SIMULADOR)";
    cout << "\n1. Crear nuevo proceso";
    cout << "\n2. Ejecutar proceso (CPU)";
    cout << "\n3. Mostrar lista de procesos";
    cout << "\n4. Mostrar cola de ejecucion";
    cout << "\n5. Mostrar pila de finalizados";
    cout << "\n0. Salir";
    cout << "\nSeleccione una opcion: ";
}
// FUNCIÓN PRINCIPAL
int main() {
    Cola cola;
    Pila pila;
    Lista lista;
    int op;

    do {
        menu();
        cin >> op;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nEntrada invalida. Intente nuevamente.\n";
            continue;
        }

        switch (op) {
            case 1: {
                Proceso *p = new Proceso;  // usamos puntero dinámico
                p->id = generarID();
                cout << "\nIngrese nombre del proceso: ";
                cin.ignore();
                getline(cin, p->nombre);
                cout << "Ingrese prioridad (1-10): ";
                cin >> p->prioridad;
                cout << "Ingrese tiempo de CPU estimado (ms): ";
                cin >> p->tiempoCPU;
                p->estado = "Listo";

                lista.insertarFinal(p);
                cola.encolar(p);
                cout << "\nProceso creado y agregado a la cola de ejecucion.\n";
                pausa();
                break;
            }

            case 2: {
                Proceso *p;
                if (cola.desencolar(p)) {
                    cout << "\nEjecutando proceso: " << p->nombre << "...\n";
                    p->estado = "Finalizado";
                    pila.push(p);
                    cout << "Proceso finalizado y enviado a la pila de terminados.\n";
                } else {
                    cout << "\nNo hay procesos en la cola.\n";
                }
                pausa();
                break;
            }

            case 3:
                lista.mostrar();
                pausa();
                break;

            case 4:
                cola.mostrar();
                pausa();
                break;

            case 5:
                pila.mostrar();
                pausa();
                break;

            case 0:
                cout << "\nSaliendo del sistema...\n";
                break;

            default:
                cout << "\nOpcion invalida.\n";
                pausa();
        }
    } while (op != 0);

    return 0;
}


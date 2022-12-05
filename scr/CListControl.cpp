#include <iostream>
#include <mutex>
#include <random>
#include <thread>

using namespace std;

mutex A;
mutex B;

//clase nodo
template <class T>
struct Nodo
{
    T valor;
    Nodo<T>* next;
    mutex mnext;
};

template <class T>
struct CList
{
    CList() { head = 0; }
    bool contains(T x, Nodo<T>**& p);//busca el valor x y el parametro p es el puntero al nodo donde se tiene que colocar en nuevo nodo en caso de insercion
    bool Add(T x);//inserta un nodo de manera ascendente y si ya existe, no la pone xd
    bool remove(T x);//elimina el nodo que buscas
    void print();
private:
    Nodo<T>* head;
};

//bloquear el nodo actual y el predecesor para modificar//se le llama block-coupling
template <class T>
bool CList<T>::contains(T x, Nodo<T>**& p)
{
    for (p = &head; *p != nullptr && (*(*p)).valor < x; p = &((*(*p)).next));


    return (*p) != nullptr && (*(*p)).valor == x;
}

template <class T>
bool CList<T>::Add(T x)
{
    Nodo<T>** p;

    if (contains(x, p))
    {
        return 0;
    }
    else {
        A.lock();
        Nodo<T>* n = new Nodo<T>;
        n->valor = x;
        n->next = *p;
        *p = n;
        A.unlock();
        return 1;
    }
}

template <class T>
bool CList<T>::remove(T x)
{
    Nodo<T>** p;
    if (!contains(x, p)) { return 0; }
    else {
        A.lock();
        Nodo<T>* t = *p;
        *p = (*p)->next;
        delete t;
        A.unlock();
        return 1;
    }
}

template <class T>
void CList<T>::print()
{


    A.lock();
    for (Nodo<T>* a = head; a != nullptr; a = a->next)
    {
        cout << a->valor << "-> ";
    }
    A.unlock();
}


//functor a al metodo add de clist
template <class T>
struct Add {
    int min;
    int max;
    CList<T>* ptr;

    Add(T min, T max, CList<T>& ptr_) {
        this->min = min;
        this->max = max;
        ptr = &ptr_;
    }

    //obtienes un random de low-high
    int get_random(int low, int high) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distribution(low, high);
        return distribution(gen);
    }

    //funcion que añade n veces un numero random del min-max a la lista ptr
    void operator()(int operaciones) {

        for (unsigned int i = 0; i < operaciones; i++) {
            ptr->Add(get_random(min, max));
        }
    }
};

//functor a al metodo add de clist
template <class T>
struct Remove {
    int min;
    int max;
    CList<T>* ptr;

    Remove(T min, T max, CList<T>& ptr_) {
        this->min = min;
        this->max = max;
        ptr = &ptr_;
    }

    //obtienes un random de low-high
    int get_random(int low, int high) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distribution(low, high);
        return distribution(gen);
    }

    //funcion que elimina n veces un numero random del min-max a la lista ptr
    void operator()(int operaciones) {

        for (unsigned int i = 0; i < operaciones; i++) {
            ptr->remove(get_random(min, max));
        }
    }
};



int main()
{
    CList<int> l;
    std::thread* oper[4];

    for (unsigned int i = 0; i < 2; i++)
    {
        oper[i] = new std::thread{ Add<int>(1,100,l),100 };//crea un thread que añade en la lista l; en la lista oper en los elementos 0 y 1
    }
    for (unsigned int i = 2; i < 4; i++)
    {
        oper[i] = new std::thread{ Remove<int>(1,100,l),100 };//crea un thread que elimina en la lista l en la lista oper en los elementos 0 y 1
    }
    for (unsigned int i = 0; i < 4; i++) {
        oper[i]->join();//hace los joins
    }

    l.print();//imprime xd
}
//aplicar thread en funcion buscar, y que atrape el anterior nodo con prev
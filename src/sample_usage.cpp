#include <iostream>
#include <cmath>
#include "quadtree.h"
using namespace std;

// A helper function
template <class T>
void print(T v){
    std::cout << v << std::endl;
}

void print(quad_tree &a){
    for (int i=0; i<pow(2, a.size()); i++){
        for (int j=0; j<pow(2, a.size()); j++)
            cout << a.get(i, j) << " ";
        cout << endl;
    }
    cout << endl;
}

int main(){

    quad_tree p(3);

    p.set(0,0,1,1,1);
    p.set(4,5,4,6,1);
    p.set(6,1,7,5,1);
    p.set(1,5,1,5,1);
    p.set(3,4,3,4,1);

    quad_tree q(3);

    q.set(0,1,1,2,1);
    q.set(5,7,6,7,1);
    q.set(3,3,3,5,1);
    q.set(6,0,7,3,1);
    q.set(0,0,1,2,1);

    quad_tree r = p;

    r.overlap(q);

    quad_tree s = p;
    s.intersect(q);

    s.complement();

    s.resize(3);
    s.set(0,0,0,0,1);
    print(s);

    s.extract(0,2,1);
    print(s);

    return 0;
}

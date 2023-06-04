#include <iostream>
#include <vector>
#include "Cubo.h"
using namespace std;
int contador_solucion=-1;
string actual_animation="N";
struct Rubick{
    //se crea un vector de cubos,crea 27 cubitos, por cada cara se necesita 9 
    vector<Cubo> cubos;
    float animation_vel=0.1;
    float reg_grados=0,cant_fps;

    vector<Cubo*> U;
    vector<Cubo*> F;
    vector<Cubo*> B;
    vector<Cubo*> L;
    vector<Cubo*> R;
    vector<Cubo*> D;
    // 3,3,-3
    // -3,-3,3
    Rubick(){
        U.reserve(9);
        F.reserve(9);
        B.reserve(9);
        L.reserve(9);
        R.reserve(9);
        D.reserve(9);

        cubos.reserve(27);
        //crea los cubos de 3x3
        for(float i=3;i!=-3;i-=2){
            for(float j=3;j!=-3;j-=2){
                for(float k=-3;k!=3;k+=2){
                    //cout<<i-2<<" "<<j-2<<" "<<k+2<<endl;
                    //cout<<i<<" "<<j<<" "<<k<<endl;
                    //cout<<"///////////////////////////////"<<endl;
                    Cubo *ptr_c=new Cubo({i-2,j-2,k+2},{i,j,k});
                    cubos.push_back(*ptr_c);
                }
            }   
        }
        //pintar verde cara 0
        //busca por las camadas y va pintando
        for(int i=0;i<3;++i){
            int p_cubo=2+(i*3);
            //cout<<p_cubo<<" "<<p_cubo+9<<" "<<p_cubo+18<<endl;
            cubos[p_cubo].change_color_cara(0,0,1,0);
            //
            cubos[p_cubo+9].change_color_cara(0,0,1,0);
            //
            cubos[p_cubo+18].change_color_cara(0,0,1,0);
            //
        }
        //pintar azul cara 1
        //cout<<"cara "<<1<<endl;
        for(int i=0;i<3;++i){
            int p_cubo=18+(i*3);
            cubos[p_cubo].change_color_cara(1,0,0,1);
            cubos[p_cubo-9].change_color_cara(1,0,0,1);
            cubos[p_cubo-18].change_color_cara(1,0,0,1);
        }
        //pintar amarillo cara 2
        for(int i=0;i<9;++i){
            int p_t=4*i;
            cubos[i].change_color_cara(2,1,1,0);
        }
        //pintar naranja cara 3
        for(int i=0;i<3;++i){
            int p_cubo=20+(i*3);
            cubos[p_cubo].change_color_cara(3,1,0.5,0);
            //
            cubos[p_cubo-1].change_color_cara(3,1,0.5,0);
            //
            cubos[p_cubo-2].change_color_cara(3,1,0.5,0);
        }
        //pintar rojo cara 4
        //cout<<"cara "<<4<<endl;
        for(int i=0;i<3;++i){
            int p_cubo=20-i;
            //cout<<p_cubo<<" "<<p_cubo+1<<" "<<p_cubo+2<<endl;
            cubos[p_cubo].change_color_cara(4,1,0,0);
            cubos[p_cubo-9].change_color_cara(4,1,0,0);
            cubos[p_cubo-18].change_color_cara(4,1,0,0);
        }
        //pintar blanco cara 5
        //cout<<"cara "<<5<<endl;
        for(int i=0;i<3;++i){
            int p_cubo=24+i;
            //cout<<p_cubo<<" "<<p_cubo-9<<" "<<p_cubo-18<<endl;
            cubos[p_cubo].change_color_cara(5,1,1,1);
            //
            cubos[p_cubo-9].change_color_cara(5,1,1,1);
            //
            cubos[p_cubo-18].change_color_cara(5,1,1,1);
        }
    }
    ///
    void colorear_cubo(int pos,float r,float g,float b){
        for(int i=0;i<6;++i){
            cubos[pos].change_color_cara(i,r,g,b);
        }
    }
    ///
    void colorear_cara_cubo(int num_cubo,int num_cara,float r,float g,float b){
        cubos[num_cubo].change_color_cara(num_cara,r,g,b);
    }
    // se modi
    void draw(GLFWwindow* window,bool wired=false){
        for(int i=0;i<27;++i){
            cubos[i].draw(window,wired);
        }
    }
    void print_eje(){

        cout<<"U: "<<U.size()<<endl;
        cout<<"F: "<<F.size()<<endl;
        cout<<"B: "<<B.size()<<endl;
        cout<<"D: "<<D.size()<<endl;
        cout<<"L: "<<L.size()<<endl;
        cout<<"R: "<<R.size()<<endl;
        cout<<"/--------------------------------/\n";
    }
    ///
    void rotate_x(float grados){
        for(int i=0;i<27;++i){
            cubos[i].rotate_x(grados);
        }
    }
    void rotate_y(float grados){
        for(int i=0;i<27;++i){
            cubos[i].rotate_y(grados);
        }
    }
};
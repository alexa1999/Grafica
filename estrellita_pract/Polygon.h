#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>
#include "Matrices.h"
using namespace std;

// ---------------------------------------------------------------------------------------------
struct Polygon
{
    vector<float> vertices;
    vector<float> vertices_borders;
    ///
    vector<float> eje_x;
    vector<float> eje_y;
    vector<float> eje_z;
    //
    int size_bits_polygon = 0;
    int points_size = 0;
    ///
    Polygon(vector<float> temp_vertices, float R, float G, float B)
    {
        vertices.reserve(temp_vertices.size() + ((temp_vertices.size() / 3) * 3));
        for (int j = 1; j <= temp_vertices.size(); ++j)
        {
            vertices.push_back(temp_vertices[j - 1]);
            //colores
            if (j % 3 == 0)
            {
                //colores
                vertices.push_back(R);
                vertices.push_back(G);
                vertices.push_back(B);
            }
        }
        size_bits_polygon = vertices.size() * sizeof(vertices);
        points_size = vertices.size() / 6;
        vertices_borders = vertices;
        for (int i = 0; i < points_size; i++)
        {
            int pos = i * 6;
            //Asigna el color negro a los bordes
            vertices_borders[pos + 3] = 0;
            vertices_borders[pos + 4] = 0;
            vertices_borders[pos + 5] = 0;
        }
        print_position();
    }
    
    
    
    ///
    void change_vertex_color(int position, float R, float G, float B)
    {
        int pos = ((position * 2) + 1) * 3;
        vertices[pos] = R;
        vertices[pos + 1] = G;
        vertices[pos + 2] = B;
    }
    ///
    void change_polygon_color(float R, float G, float B)
    {
        for (int i = 0; i < points_size; i++)
        {
            int pos = i * 6;
            vertices[pos + 3] = R;
            vertices[pos + 4] = G;
            vertices[pos + 5] = B;
        }
    }
    void act_min_max_x(){
        vector<float> result;
        result.reserve(2);
        result.resize(2);
        result[0] = vertices[0]; //min
        result[1] = vertices[0]; //max
        for (int i = 1; i < points_size; i++)
        {
            int pos = i * 6;
            if (vertices[pos] < result[0])
                result[0] = vertices[pos];
            if (vertices[pos] > result[1])
                result[1] = vertices[pos];
        }
        eje_x = result;
    }
    ///
    void act_min_max_y(){
        vector<float> result;
        result.reserve(2);
        result.resize(2);
        result[0] = vertices[1]; //min
        result[1] = vertices[1]; //max
        for (int i = 1; i < points_size; i++)
        {
            int pos = i * 6;
            if (vertices[pos + 1] < result[0])
                result[0] = vertices[pos + 1];
            if (vertices[pos + 1] > result[1])
                result[1] = vertices[pos + 1];
        }
        eje_y = result;
    }
    ///
    void act_min_max_z(){
        vector<float> result;
        result.reserve(2);
        result.resize(2);
        result[0] = vertices[2]; //min
        result[1] = vertices[2]; //max
        for (int i = 1; i < points_size; i++)
        {
            int pos = i * 6;
            if (vertices[pos + 2] < result[0])
                result[0] = vertices[pos + 2];
            if (vertices[pos + 2] > result[1])
                result[1] = vertices[pos + 2];
        }
        eje_z = result;
    }
    void actualizar_ejes(){
        act_min_max_x();
        act_min_max_y();
        act_min_max_z();
    }
    ///se modifico, se copian los vertices para dibujar lineas en los poligonos
    void draw(GLFWwindow *window, bool wired=false){
        glBufferData(GL_ARRAY_BUFFER, size_bits_polygon, vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_FAN, 0, points_size);
        //borders, como el poligono es cerrado,el primitivo ayuda a que se cierre la linea.
        if(wired==false){
            glLineWidth(8);
            glBufferData(GL_ARRAY_BUFFER, size_bits_polygon, vertices_borders.data(), GL_DYNAMIC_DRAW);
            glDrawArrays(GL_LINE_LOOP, 0, vertices_borders.size()/6);   
        }
    }
    ///
    void noob_translate(float new_x,float new_y,float new_z){
        for(int i=0;i<points_size;i++){
            int pos=i*6;
            vertices[pos]+=new_x;
            vertices[pos+1]+=new_y;
            vertices[pos+2]+=new_z;
        }
    }
    void pro_traslate(vector<float> point)
    { //ready
        Matrix m1(4, 4);
        Matrix m2(vertices);
        Matrix m3(4, 4);
        m1.create_matrix_point(point);
        Multiply(m1, m2, m3);
        m3.export_to_vertex(vertices);
        m3.export_to_vertex(vertices_borders);
    }
    void pro_translate_side(float distance) {
        for (int i = 0; i < points_size; i++) {
            vertices[i * 3] += distance; // Añade 'distance' a la coordenada x de cada vértice
        }
    }
        ///
    void pro_shear_origin(float angle)
    { //ready
        Matrix m1(3, 3);
        Matrix m2(vertices);
        Matrix m3(3, 3);

        m1.convert_matrix_rotation_origin(angle);
        //m1.print_matrix();
        Multiply(m1, m2, m3);
        m3.export_to_vertex(vertices);
    }
    ///
    void pro_rotation_2d_x(float angle)
    {
        Matrix m1(3, 3);
        Matrix m2(vertices);
        Matrix m3(3, points_size);

        m1.convert_matrix_rotation_2d_x(angle);
        Multiply(m1, m2, m3);
        m3.export_to_vertex(vertices);
    }
    ///
    void pro_rotation_x(float angle)
    {
        Matrix m1(4, 4);
        Matrix m2(vertices);
        Matrix m3(4, points_size);

        m1.convert_matrix_rotation_x(angle);
        Multiply(m1, m2, m3);
        m3.export_to_vertex(vertices);
        m3.export_to_vertex(vertices_borders);
    }
    ///
    void pro_rotation_2d_y(float angle)
    {
        Matrix m1(3, 3);
        Matrix m2(vertices);
        Matrix m3(3, points_size);

        m1.convert_matrix_rotation_2d_y(angle);
        Multiply(m1, m2, m3);
        m3.export_to_vertex(vertices);
    }
    ///
    void pro_rotation_y(float angle)
    {
        Matrix m1(4, 4);
        Matrix m2(vertices);
        Matrix m3(4, points_size);

        m1.convert_matrix_rotation_y(angle);
        //m1.print_matrix();
        Multiply(m1, m2, m3);

        m3.export_to_vertex(vertices);
        m3.export_to_vertex(vertices_borders);
    }
    ///
    void pro_rotation_z(float angle)
    {
        Matrix m1(4, 4);
        Matrix m2(vertices);
        Matrix m3(4, points_size);

        m1.convert_matrix_rotation_z(angle);
        //m1.print_matrix();
        Multiply(m1, m2, m3);

        m3.export_to_vertex(vertices);
        m3.export_to_vertex(vertices_borders);
    }
    void noob_rotar(float punto_rotacion_x,float punto_rotacion_y, float angulo){
        for(int i=0;i<points_size;i++){
            int pos=i*6;

            float diferencia_x = vertices[pos] - punto_rotacion_x;
            float diferencia_y = vertices[pos+1] - punto_rotacion_y;

            float radio=sqrt((diferencia_x * diferencia_x)+(diferencia_y * diferencia_y));
            float da_rad = angulo / 180 * PI;
            float angulo = atan2(diferencia_y,diferencia_x) - da_rad;


            
            vertices[pos]= punto_rotacion_x + radio * cos(angulo);
            vertices[pos+1]= punto_rotacion_y +  radio * sin(angulo);
        }
    }
    // void rotar(float &x, float &y, float cx, float cy, float ang)
    // {
    //     float dx = x - cx;
    //     float dy = y - cy;
    //     float r = sqrt(dx * dx + dy * dy);
    //     float a = atan2(dy, dx);

    //     float da_rad = ang / 180 * M_PI;
    //     a -= da_rad;
    //     x = cx + r * cos(a);
    //     y = cy + r * sin(a);
    // }

    // void rotar_todo(float arr[], int n, int angle)
    // {
    //     for (int i = 0; i < n / 3; ++i)
    //     {
    //         rotar(Acceso(arr, i * 3 + 0), Acceso(arr, i * 3 + 1), 0, 0, angle);
    //     }
    // }
    ///
    /*
    void pro_rotate1(float angulo, vector<float> eje) {
        float rad = angulo * PI / 180.0;
        float c = cos(rad);
        float s = sin(rad);

        Matrix m1(4, 4);
        Matrix m2(vertices);
        Matrix m3(4, 4);

        float x = eje[0];
        float y = eje[1];
        float z = eje[2];

        m1.create_matrix_rotation1(x, y, z, c, s);
        Multiply(m1, m2, m3);
        m3.export_to_vertex(vertices);
    }*/
    void pro_scale(float x, float y, float z)
    {
        Matrix m1(4, 4);
        Matrix m2(vertices);
        Matrix m3(4, points_size);

        m1.convert_matrix_scale(x, y, z);
        //m1.print_matrix();
        Multiply(m1, m2, m3);
        m3.print_matrix();
        m3.export_to_vertex(vertices);
    }
    void print_position()
    {
        char ejes[3] = {'x', 'y', 'z'};
        for (int i = 0; i < points_size; i++)
        {
            int pos = i * 6;
            cout << ejes[i] << ": ";
            cout << vertices[pos] << "\t";
            cout << vertices[pos + 1] << "\t";
            cout << vertices[pos + 2] << endl;
        }
        cout << "///////////////////////\n";
    }
};
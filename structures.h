#pragma once
#include<string.h>
#include<queue>
#include<stdio.h>


struct adjacency_matrix{
    int **matrix;
    int number_of_vertices;
    adjacency_matrix(){
        matrix = new int*[0];
        matrix[0] = new int[0];
        number_of_vertices = 0;
    };
    adjacency_matrix(int number_of_vertices){
        matrix = new int*[number_of_vertices];
        for(int i = 0; i < number_of_vertices; i++){
            matrix[i] = new int[number_of_vertices];
            memset(matrix[i],0,number_of_vertices*sizeof(int));
        }
        this->number_of_vertices = number_of_vertices;
    };
    void print(){
        printf("   |");
        for(int i = 0; i < this->number_of_vertices; i++){
            printf("%3d ", i);
        }
        printf("\n----");
        for(int i = 0; i < this->number_of_vertices; i++){
            printf("----", i);
        }
        printf("\n");
        for(int i = 0; i < this->number_of_vertices; i++){
            printf("%3d|",i);
            for(int j = 0; j < this->number_of_vertices; j++){
                printf("%3d ",this->matrix[i][j]); //matrix[row][col]
            }
            printf("\n");
        }
    }
    void add_vertex(){
        this->number_of_vertices = this->number_of_vertices+1;
        int **temp_matrix = new int*[this->number_of_vertices];
        memcpy(temp_matrix,this->matrix,(this->number_of_vertices-1)*sizeof(int));
        for(int i = 0; i < this->number_of_vertices-1; i++){
            temp_matrix[i] = new int[number_of_vertices];
            memcpy(temp_matrix[i],this->matrix[i],(this->number_of_vertices-1)*sizeof(int));
            temp_matrix[i][number_of_vertices-1] = 0;
        }
        temp_matrix[number_of_vertices-1] = new int[number_of_vertices];
        memset(temp_matrix[number_of_vertices-1],0,number_of_vertices*sizeof(int));
        delete[] this->matrix;
        this->matrix = temp_matrix;
    };
    bool add_edge_dir(int src_vertex, int dst_vertex, int weight = 1){
        if(src_vertex < this->number_of_vertices && dst_vertex < this->number_of_vertices && src_vertex >= 0 && dst_vertex >= 0){
            this->matrix[src_vertex][dst_vertex] = weight;
            return true;
        }else{
            return false;
        }
    };
    bool add_edge_undir(int src_vertex, int dst_vertex, int weight = 1){
        if(src_vertex < this->number_of_vertices && dst_vertex < this->number_of_vertices && src_vertex >= 0 && dst_vertex >= 0){
            this->matrix[src_vertex][dst_vertex] = weight;
            this->matrix[dst_vertex][src_vertex] = weight;
            return true;
        }else{
            return false;
        }
    };
};
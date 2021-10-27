#include <iostream>
#include <string.h>
#include <fstream>
#include <stdio.h>
#include <queue>
#include <chrono>
#include <vector>
// #include <conio.h>
#include <random>
#include <string>
// #include <windows.h>
#include <sstream>
#include <climits>
#include <iterator>
#include <algorithm>
#include <cctype>
#include <locale>
#include <cmath>
#include <tuple>
#include <unordered_set>
#include "structures.h"
using namespace std;
using namespace std::chrono;

vector<vector<string>> tasks;
vector<string> results;
string results_file_name = "";
int number_of_current_graph_vertices = 0;
adjacency_matrix current_graph_adjacency_matrix = adjacency_matrix();
int *path_array;

struct Result
{
    string graph_name;
    string path;
    int path_weight;
    double time;
    int number_of_repeats;
    Result(string graph_name, string path, int path_weight, double time, int number_of_repeats)
    {
        this->graph_name = graph_name;
        this->path = path;
        this->path_weight = path_weight;
        this->time = time;
        this->number_of_repeats = number_of_repeats;
    }
    string toString()
    {
        return (graph_name + "," + path + "," + to_string(path_weight) + "," + to_string(time) + "," + to_string(number_of_repeats));
    }
};

void save_results(string results_file_name)
{
    std::cout << "Saving results" << endl;
    fstream fout;
    fout.open(results_file_name, ios::out);
    fout << "graph_name,path,path_weight,time,number_of_repeats" << endl;
    for (int i = 0; i < results.size(); i++)
    {
        fout << results[i] << endl;
    }
    fout.close();
    std::cout << "Correctly saved " << results.size() << " results" << endl;
}

template <typename Out>
void split(const std::string &s, char delim, Out result)
{
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim))
    {
        *result++ = item;
    }
}
std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

// trim from start (in place)
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                    { return !std::isspace(ch); }));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                         { return !std::isspace(ch); })
                .base(),
            s.end());
}

bool load_data(string file_name)
{
    std::cout << "Loading data from " << file_name << " file" << endl;
    ifstream fin;
    fin.open(file_name, ios::in);
    if (!fin.good())
    {
        std::cout << "Data file " << file_name << " not exist" << endl;
        fin.close();
        return false;
    }
    string loaded_source, loaded_destination, loaded_weight;
    string loaded_number_of_vertices;
    getline(fin, loaded_number_of_vertices);
    number_of_current_graph_vertices = stoi(loaded_number_of_vertices);
    current_graph_adjacency_matrix = adjacency_matrix(number_of_current_graph_vertices);
    for (int i = 0; i < number_of_current_graph_vertices; i++)
    {
        string loaded_line_of_matrix = "";
        getline(fin, loaded_line_of_matrix);
        ltrim(loaded_line_of_matrix);
        rtrim(loaded_line_of_matrix);
        vector<string> single_line = split(loaded_line_of_matrix, ' ');
        std::vector<std::string>::iterator it = single_line.begin();
        while (it != single_line.end())
        {
            if (it->length() == 0)
            {
                it = single_line.erase(it);
            }
            else
            {
                ++it;
            }
        }
        for (int j = 0; j < single_line.size(); j++)
        {
            ltrim(single_line[j]);
            rtrim(single_line[j]);
            current_graph_adjacency_matrix.add_edge_dir(i, j, stoi(single_line[j]));
        }
    }
    std::cout << "Loaded correctly graph with " << number_of_current_graph_vertices << " vertices" << endl
              << "Graph:" << endl;
    current_graph_adjacency_matrix.print();
    fin.close();
    return true;
}

void load_config()
{
    std::cout << "Loading config.ini" << endl;
    ifstream fin;
    fin.open("config.ini", ios::in);
    if (!fin.good())
    {
        std::cout << "Config.ini not found" << endl;
        fin.close();
        return;
    }
    string loaded_line_of_task = "";
    getline(fin, results_file_name);
    while (getline(fin, loaded_line_of_task))
    {
        vector<string> single_line = split(loaded_line_of_task, ' ');
        string graph_file_name, test_type, number_of_repeats, shortest_path_weight, shortest_path;
        if (single_line.size() >= 5)
        {
            graph_file_name = single_line[0];
            test_type = single_line[1];
            number_of_repeats = single_line[2];
            shortest_path_weight = single_line[3];
            for (int i = 4; i < single_line.size(); i++)
            {
                shortest_path += single_line[i];
                shortest_path += " ";
            }
        }
        if (graph_file_name.size() == 0 || test_type.size() == 0 || number_of_repeats.size() == 0 || shortest_path_weight.size() == 0 || shortest_path.size() == 0)
        {
            std::cout << "Cannot load this task: " << graph_file_name << " " << test_type << " " << number_of_repeats << " " << shortest_path_weight << " " << shortest_path;
            break;
        }
        vector<string> task;
        task.push_back(graph_file_name);
        task.push_back(test_type);
        task.push_back(number_of_repeats);
        task.push_back(shortest_path_weight);
        task.push_back(shortest_path);
        tasks.push_back(task);
    }
    fin.close();
    std::cout << "Config loaded correctly" << endl;
    return;
}

unsigned int countSetBits(unsigned int n)
{
    unsigned int count = 0;
    while (n)
    {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

int fact(int x)
{
    if (x == 1 || x == 0)
        return 1;
    return (x * fact(x - 1));
}

int *solve_combination(int finish, unsigned long long node_set)
{ // zwraca tablice kombinacji w układzie: koszt, rodzic, w docelowy, kolejne elementy kombinacji
    if (node_set == 0)
    {
        int *solved_combinations = new int[3];
        solved_combinations[0] = current_graph_adjacency_matrix.matrix[0][finish];
        solved_combinations[1] = 0;
        solved_combinations[2] = finish;
        return solved_combinations;
    }
    else
    {
        int node_set_quantity = countSetBits(node_set);
        int *solved_combinations = new int[3]; //local min / parent / finish
        solved_combinations[0] = INT_MAX; // local min
        solved_combinations[1] = 0; // local_min_parent;
        solved_combinations[2] = finish;
        for (int i = 0; i < sizeof(node_set) * 8; i++)
        {
            if (1 & (node_set >> i)) // czy i nie należy do node_set
            {
                unsigned long long current_node_set = node_set & (~(1 << i));
                int* current_solve = solve_combination(i + 1, current_node_set);
                int cost = current_graph_adjacency_matrix.matrix[i + 1][finish] + current_solve[0];
                if (cost < solved_combinations[0])
                {
                    solved_combinations[0] = cost;
                    solved_combinations[1] = i + 1;
                }
                delete current_solve;
            }
        }
        return solved_combinations;
    }
}

pair<vector<int>, int> TSP_held_karp()
{
    vector<int> path;
    int weight;
    unsigned long long node_set;
    //stworzenie maski końcowej kombinacji czyli wszystkie wierzchołki poza początkowym
    node_set = pow(2, number_of_current_graph_vertices - 1) - 1;
    int *solve = solve_combination(0, node_set);
    weight = solve[0];
    path.push_back(0);
    int *temp_solve = solve;
    unsigned long long temp_node_set = node_set;
    for(int i = 0; i < number_of_current_graph_vertices; i++){
        path.push_back(temp_solve[1]);
        temp_node_set = temp_node_set & (~(1 << (temp_solve[1]-1)));
        temp_solve = solve_combination(temp_solve[1], temp_node_set);
    }
    std::reverse(path.begin(), path.end());
    return make_pair(path, weight);
}
int main()
{
    load_config();
    if (tasks.size() == 0)
    {
        std::cout << "No tasks found to be performed." << endl;
    }
    else
    {
        for (int i = 0; i < tasks.size(); i++)
        {
            std::cout << endl
                      << "##################################################" << endl
                      << endl;
            string graph_file_name = tasks[i][0];
            string test_type = tasks[i][1];
            int number_of_repeats = stoi(tasks[i][2]);
            string shortest_path_weight = tasks[i][3];
            string shortest_path = tasks[i][4];
            if (!load_data(graph_file_name))
            {
                std::cout << "Cannot load graph from " << graph_file_name << " file." << endl;
            }
            std::cout << "Computing TSP in " << graph_file_name << " graph repeated " << number_of_repeats << " times" << endl;
            if (number_of_current_graph_vertices < 1)
            {
                std::cout << "Cannot execute task. The array must to have at least 1 element.";
            }
            else if (number_of_repeats < 1)
            {
                std::cout << "Cannot execute task. The minimum number of repetitions is 1.";
            }
            else
            {
                pair<vector<int>, int> answer;
                high_resolution_clock::time_point t_start = high_resolution_clock::now();
                for (int j = 0; j < number_of_repeats; j++)
                {
                    answer = TSP_held_karp();
                }
                high_resolution_clock::time_point t_end = high_resolution_clock::now();
                duration<double> time_span = duration_cast<duration<double>>(t_end - t_start);
                int weight = answer.second;
                string path = "";
                std::vector<int>::iterator it = answer.first.begin();
                while (it != answer.first.end())
                {
                    path += to_string(*it);
                    path += " ";
                    it++;
                }
                ltrim(path);
                rtrim(path);
                cout << "Calculated shortest path: " << path << endl
                     << "Defined shortest path:    " << shortest_path << endl
                     << "Calculated weight: " << weight << endl
                     << "Defined weight:    " << shortest_path_weight << endl
                     << "Time: " << ((double)time_span.count() / (double)number_of_repeats) << " s" << endl;
                Result result = Result(graph_file_name, path, weight, time_span.count(), number_of_repeats);
                results.push_back(result.toString());
            }
        }
    }
    std::cout << endl;
    save_results(results_file_name);
    std::cout << "Press any key to continue...";
    getchar();
    return 0;
}
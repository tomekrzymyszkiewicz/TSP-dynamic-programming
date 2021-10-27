#include <iostream>
#include <string.h>
#include <fstream>
#include <stdio.h>
#include <queue>
#include <chrono>
#include <vector>
#include <conio.h>
#include <random>
#include <string>
#include <windows.h>
#include <sstream>
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

int fact(int x)
{
    if (x == 1 || x == 0)
        return 1;
    return (x * fact(x - 1));
}

void generate(int k, vector<bool> &my_set, int N, vector<vector<int>> &current_subsets)
{
    if (k == N)
    {
        vector<int> current_subset;
        for (int i = 0; i < N; i++)
        {
            if (my_set[i] == true)
                current_subset.push_back(i + 1);
        }
        current_subsets.push_back(current_subset);
    }
    else
    {
        my_set[k] = true;
        generate(k + 1, my_set, N, current_subsets);
        my_set[k] = false;
        generate(k + 1, my_set, N, current_subsets);
    }
}

vector<vector<int>> generate_subsets(int n)
{
    vector<vector<int>> subsets;
    vector<bool> my_set(n);
    generate(0, my_set, n, subsets);
    std::sort(subsets.begin(), subsets.end(), [](const vector<int> &a, const vector<int> &b)
              { return a.size() < b.size(); });
    return subsets;
}

pair<vector<int>, int> TSP_held_karp()
{
    int number_of_combinations = pow(2, number_of_current_graph_vertices - 1);
    vector<vector<int>> subsets = generate_subsets(number_of_current_graph_vertices - 1);
    vector<tuple<pair<int, vector<int>>, int, int>> map_of_costs_and_parents;
    vector<int> path;
    int number_of_used_combinations = 0;
    for (int i = 0; i < number_of_combinations; i++) // i numer zbioru
    {
        int prev_subs_len = subsets[i].size() - 1;
        int prepared_combinations = map_of_costs_and_parents.size();
        int number_of_prev_combinations = 0;
        if (i > 0)
        {
            number_of_prev_combinations = fact(number_of_current_graph_vertices - 1) / (fact(prev_subs_len) * fact(number_of_current_graph_vertices - prev_subs_len - 2));
            if (subsets[i].size() > subsets[i - 1].size() && i > 1)
            {
                number_of_used_combinations = prepared_combinations - number_of_prev_combinations;
            }
        }

        for (int j = 1; j < number_of_current_graph_vertices; j++) // j numer wierzchołka w grafie (do którego idziemy) wybrany z podzbioru
        {
            vector<int>::iterator it = std::find(subsets[i].begin(), subsets[i].end(), j);
            if (it == subsets[i].end()) //wykonuj jeśli j nie znajduje się w podzbiorze
            {
                int local_min_cost = INT_MAX;
                int parent = 0;
                for (int k = 0; k < subsets[i].size(); k++)
                { // k numer elementu w bieżąco analizowanym zbiorze
                    int prev_cost = 0;
                    // for (auto &element : map_of_costs_and_parents)
                    for (vector<tuple<pair<int, vector<int>>, int, int>>::iterator it = map_of_costs_and_parents.begin() + number_of_used_combinations; it != map_of_costs_and_parents.begin() + number_of_used_combinations + number_of_prev_combinations; ++it)
                    {
                        //znajdź koszt poprzednika
                        vector<int> prev_subset = subsets[i];
                        prev_subset.erase(remove(prev_subset.begin(), prev_subset.end(), subsets[i][k]));
                        bool subsets_comparsion = std::equal(get<0>(*it).second.begin(), get<0>(*it).second.end(), prev_subset.begin(), prev_subset.end());
                        if (get<0>(*it).first == subsets[i][k] && subsets_comparsion)
                        {
                            prev_cost = get<1>(*it);
                            break;
                        }
                    }
                    int cost = current_graph_adjacency_matrix.matrix[subsets[i][k]][j] + prev_cost;
                    if (cost < local_min_cost)
                    {
                        local_min_cost = cost;
                        parent = subsets[i][k];
                    }
                }
                if (local_min_cost == INT_MAX)
                {
                    local_min_cost = current_graph_adjacency_matrix.matrix[0][j];
                }
                map_of_costs_and_parents.push_back(make_tuple(make_pair(j, subsets[i]), local_min_cost, parent));
            }
        }
    }
    int final_local_min_cost = INT_MAX;
    int final_parent = 0;
    // for (auto &element : map_of_costs_and_parents)
    for (vector<tuple<pair<int, vector<int>>, int, int>>::iterator it = map_of_costs_and_parents.begin() + map_of_costs_and_parents.size() - number_of_current_graph_vertices + 1; it != map_of_costs_and_parents.end(); ++it)
    {
        if (get<0>(*it).second.size() == number_of_current_graph_vertices - 2)
        {
            int cost = get<1>(*it) + current_graph_adjacency_matrix.matrix[get<0>(*it).first][0];
            if (cost < final_local_min_cost)
            {
                final_local_min_cost = cost;
                final_parent = get<0>(*it).first;
            }
        }
    }
    map_of_costs_and_parents.push_back(make_tuple(make_pair(0, subsets[subsets.size() - 1]), final_local_min_cost, final_parent));
    path.push_back(0);
    for (int i = 5; i > 0; i--)
    {
        int local_min_cost = INT_MAX;
        int parent_with_min_cost = 0;
        for (auto &element : map_of_costs_and_parents)
        {
            if (get<0>(element).second.size() == i && get<0>(element).first == path[path.size() - 1])
            {
                if (get<1>(element) < local_min_cost)
                {
                    local_min_cost = get<1>(element);
                    parent_with_min_cost = get<2>(element);
                }
            }
        }
        path.push_back(parent_with_min_cost);
    }
    path.push_back(0);
    int weight = get<1>(map_of_costs_and_parents[map_of_costs_and_parents.size() - 1]);
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
    getch();
    return 0;
}
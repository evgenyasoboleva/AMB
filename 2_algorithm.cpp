/*
 * Пыталась я привинтить метод ветвей и границ для задачи коммивояжера к этой задаче, получилось очень не очень
 * (не доделала нормальный вывод до конца, потому что уже видно, что нехорошо).
 */

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

/*
 * Функция вычисляет максимальную длину суффикса строки s1
 * совпадающего с префиксом строки s2 (длину наложения s1 на s2)
 */
int Overlap (const string& s1, const string& s2)
{
    int overlap = 0;
    for (int i = s1.length() - 1, j = 1; i >= 0 && j <= s2.length(); i--, j++)
    {
        string suff = s1.substr(i);
        string pref = s2.substr(0, j);
        if (suff == pref)
            overlap = j;
    }
    return overlap;
}

/*
 * Вычисление матрицы длины суффиксов для всех
 * упорядоченных пар (Si, Sj).
 */

int** SuffixLengthMatrix (const vector <string>& strings, size_t n)
{
    int **suffix_length = new int* [n];
    for (int i = 0;  i < n; i++)
        suffix_length[i] = new int [n];

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i != j)
                suffix_length[i][j] = strings[j].length() - Overlap(strings[i], strings[j]);
            else
                suffix_length[i][j] = 0;
        }
    }
    return suffix_length;
}

/*
 * Находим 0, которому соответствует максимальный штраф
 */

int MaxPenalty (int** matrix, size_t size, vector<vector<int>>& zeros, bool** notallow)
{
    int min, penalty = 0, max_penalty = 0, zero_number = 0;

    for (int i = zeros.size() - 1; i >= 0; i--)
    {
        int j = 0;
        while(notallow[zeros[i][0]][j] && j < size)
            j++;
        if (j == size)
            continue;
        else
            min = matrix[zeros[i][0]][j];

        for (j = 0; j < size; j++){
            if (min > matrix[zeros[i][0]][j] && !notallow[zeros[i][0]][j])
                min = matrix[zeros[i][0]][j];
        }
        penalty += min;

        j = 0;
        while(notallow[j][zeros[i][1]] && j < size)
            j++;
        if (j == size)
            continue;
        else
            min = matrix[j][zeros[i][1]];

        for (j = 0; j < size; j++){
            if (min > matrix[j][zeros[i][1]] && !notallow[j][zeros[i][1]])
                min = matrix[j][zeros[i][1]];
        }
        penalty += min;

        if (penalty > max_penalty) {
            max_penalty = penalty;
            zero_number = i;
        }
        zeros[i].push_back(max_penalty);
    }
    return zero_number;
}

/*
 * Вычисление оценки затрат
 */

vector<int> Cost(int** matrix, size_t size, bool** notallow)
{
    int min, r = 0;
    vector<vector<int>> zeros;

    for (int i = 0; i < size; i++)
    {
        int j = 0;
        while (notallow[i][j] && j < size)
            j++;
        if (j == size)
            continue;
        else
            min = matrix[i][j];

        for (j = 0; j < size; j++) {
            if (matrix[i][j] < min && !notallow[i][j])
                min = matrix[i][j];
        }
        for (j = 0; j < size; j++) {
            if (!notallow[i][j])
                matrix[i][j] = matrix[i][j] - min;
        }
        r += min;
    }

    for (int i = 0; i < size; i++)
    {
        int j = 0;
        while (notallow[j][i] && j < size - 1)
            j++;
        if (notallow[j][i])
            continue;
        else
            min = matrix[j][i];

        for (j = 0; j < size; j++) {
            if (matrix[j][i] < min && !notallow[j][i])
                min = matrix[j][i];
        }
        for (j = 0; j < size; j++)
        {
            if (!notallow[j][i])
                matrix[j][i] = matrix[j][i] - min;
            if (matrix[j][i] == 0 && !notallow[j][i]) {
                vector<int> zero = {j, i};
                zeros.push_back(zero);
            }
        }
        r += min;
    }
    int zero_number = MaxPenalty(matrix, size, zeros, notallow);
    
    return {zeros[zero_number][2], zeros[zero_number][0], zeros[zero_number][1], r};
}


vector<vector<int>> Result (int** matrix, size_t size)
{
    int** input_matrix = matrix;
    vector <vector<int>> result;
    int t = size, r = 0;

    bool** notallow = new bool*[size];
    for (int i = 0; i < size; i++)
        notallow[i] = new bool[size];

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++) {
            if (i == j)
                notallow[i][j] = true;
            else
                notallow[i][j] = false;
        }
    }

    while (t != 1)
    {
        vector <int> cost_result_1 = Cost(matrix, size, notallow);
        r += cost_result_1[3]; //сумма затрат на приведение
        int s_1 = r + cost_result_1[0]; // для множества, не содержащего ребро
        int n_string = cost_result_1[1]; //номер строки
        int n_column = cost_result_1[2]; //номер стобца

        bool** new_notallow = notallow;
        for (int i = 0; i < size; i++){
            new_notallow[n_string][i] = true;
            new_notallow[i][n_column] = true;
        }
        new_notallow[n_column][n_string] = true;

        vector<int> cost_result_2 = Cost(matrix, size, new_notallow);
        int s_2 = r + cost_result_2[0];

        if (s_1 < s_2)
            notallow[n_string][n_column] = true;
        else {
            notallow = new_notallow;
            t--;
            vector<int> edge = {n_string, n_column};
            result.push_back(edge);
        }
    }

    for (int i = 0;  i < size; i++)
        delete[] notallow[i];
    delete[] notallow;

    return result;
}


int main() {
    string str;
    vector <string> strings;

    ifstream input ("/home/evgen/CLionProjects/untitled1/strings.txt");

    if (input){
        while (getline(input, str, ' '))
            strings.push_back(str);
    }
    else
        cout << "Problems!";

    strings[strings.size() - 1].resize(strings[strings.size() - 1].size() - 1);

    size_t size = strings.size();
    int** suffix_matrix = SuffixLengthMatrix (strings, strings.size());
    vector <vector<int>> result = Result(suffix_matrix, strings.size());

    for (vector <int> s: result) {
        for (int c: s)
            cout << c << " ";
        cout << endl;
    }

    for (int i = 0;  i < strings.size(); i++)
        delete[] suffix_matrix[i];
    delete[] suffix_matrix;

    return 0;
}

#include <iostream>
#include <vector>
#include <algorithm>
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
 * Вычисление матрицы overlap'ов для всех
 * упорядоченных пар (Si, Sj).
 */

int** OverlapsMatrix (const vector <string>& strings, size_t n)
{
    int **overlaps = new int* [n];
    for (int i = 0;  i < n; i++)
        overlaps[i] = new int [n];

    for (int i = 0; i < n; i++)
        for (int j = i; j < n; j++) {
            if (i != j) {
                overlaps[i][j] = Overlap(strings[i], strings[j]);
                overlaps[j][i] = Overlap(strings[j], strings[i]);
            }
            else
                overlaps[i][j] = 0;
        }
    return overlaps;
}


/*
* Функция, вычисляющая полное назначение максимального
* веса жадным методом. Время - O(k*k*log(k))
*/
int* Assignment(int** a, size_t n)
{
    bool notallow [n][n];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            notallow[i][j] = false;

    int* m = new int [n];
    for (int i = 0; i < n; i++)
        m[i] = 0;

    while (true)
    {
        int max = -1, maxi = -1, maxj = -1;

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (notallow[i][j]) continue;
                if (a[i][j] > max)
                {
                    max = a[i][j];
                    maxi = i; maxj = j;
                }
            }
        }

        if (max == -1) break;

        m[maxi] = maxj;

        for (int i = 0; i < n; i++)
        {
            notallow[maxi][i] = true;
            notallow[i][maxj] = true;
        }
    }
    return m;
}


/*
 * Нахождение покрытия циклами минимальной полной длины
 * для полного назначения
*/
vector<vector<int>> CyclesSearch (int* assign, size_t n)
{
    vector<vector<int>> cycles;
    vector<int> cycle;

    bool mark [n];
    for (int j = 0; j < n; j++)
        mark[j] = false;

    for (int i = 0; i < n; i++)
    {
        if (mark[i]) continue;

        cycle.push_back(i);
        mark[i] = true;

        if (assign[i] == cycle[0])
        {
            cycles.push_back(cycle);
            cycle.clear();
            i = 0;
        }
        else
            i = assign[i] - 1;
    }
    return cycles;
}


/*
 * Функция возвращает строку s1, обрезанную справа
 * на overlap символов
*/
string Prefix (string s1, int overlap)
{
    return s1.substr(0, s1.length() - overlap);
}


/*
 * Циклический сдвиг каждого цикла в покрытии такой, чтобы
 * минимизировать overlap первой и последней строки в цикле
 * и конструирование надстроки для каждого цикла, результат.
*/
string Construction (const vector<string>& strings){
    size_t strings_len = strings.size();

    int **overlaps = OverlapsMatrix(strings, strings.size());

    int * assign = Assignment(overlaps, strings_len);

    vector<vector<int>> cycles = CyclesSearch(assign, strings_len);

    vector<string> superstrings;

    for (vector<int> c: cycles)
    {
        string str = "";
        vector<int> cycle_overlaps;

        for (int i = 0; i < c.size() - 1; i++)
            cycle_overlaps.push_back(overlaps[c[i]][c[i + 1]]);

        int min = overlaps[c[c.size() - 1]][c[0]];
        int shift = 0;

        for (int i = 0; i < cycle_overlaps.size(); i++)
            if (cycle_overlaps[i] < min){
                min = cycle_overlaps[i];
                shift = i + 1;
            }

        rotate(c.begin(), c.begin() + shift, c.end());

        for (int i = 0; i < c.size() - 1; i++)
            str += Prefix(strings[c[i]], overlaps[c[i]][c[i + 1]]);
        str += strings[c[c.size() - 1]];

        superstrings.push_back(str);
    }

    string result = "";
    for (string str: superstrings)
        result += str;

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

    cout << Construction (strings);

    return 0;
}

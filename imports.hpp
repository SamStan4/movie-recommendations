#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#define RATINGS_FILE_PATH "./movie-lens-data/ratings.csv"
#define GIVEN_NEIGHBORHOOD_SIZE 5

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::getline;
using std::stoi;
using std::stod;
using std::sqrt;
using std::sort;
using std::reverse;
using std::max;

#define RATINGS_FILE_PATH "./movie-lens-data/ratings.csv"
#define GIVEN_NEIGHBORHOOD_SIZE 5

void split_line(vector<string>& dest, string line, char delimeter)
{
    dest.clear();
    string temp = "";
    for (char c : line)
    {
        if (c == delimeter)
        {
            if (temp.size() > 0)
            {
                dest.push_back(temp);
                temp.clear();
            }
        } 
        else
        {
            temp.push_back(c);
        }
    }

    if (temp.size() > 0)
    {
        dest.push_back(temp);
    }
}

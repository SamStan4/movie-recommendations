#pragma once

#include "item_sim_pair.hpp"

class recommender_wrapper
{
private:
    unordered_map<int, int> movie_id_to_row_index;
    unordered_map<int, int> row_index_to_movie_id;
    unordered_map<int, int> user_id_to_col_index;
    unordered_map<int, int> col_index_to_user_id;
    vector<vector<double>> rating_matrix;
    vector<vector<bool>> is_rated_matrix;
    vector<vector<item_sim_pair>> similarity_matrix;
    int total_number_movies;
    int total_number_users;


public:

    recommender_wrapper()
    {
        // populates all of the hash tables
        this->populate_maps();


    }

    void populate_maps(void)
    {
        unordered_set<int> user_id_set, movie_id_set;
        ifstream input_stream(RATINGS_FILE_PATH, std::ios::in);
        vector<string> line_split = {};
        string line = "";

        if (!input_stream.is_open())
        {
            cout << RATINGS_FILE_PATH << " not found" << endl;
            exit(1);
        }

        getline(input_stream, line); // read the file header

        while (getline(input_stream, line))
        {
            split_line(line_split, line, ',');

            user_id_set.insert(stoi(line_split[0]));
            movie_id_set.insert(stoi(line_split[1]));
        }

        input_stream.close();

        int i = 0;
        for (auto it = movie_id_set.begin(); it != movie_id_set.end(); ++it)
        {
            this->movie_id_to_row_index[*it] = i; 
        }
        
        i = 0;
        for (auto it = user_id_set.begin(); it != user_id_set.end(); ++it)
        {
            this->user_id_to_col_index[*it] = i;
        }

        this->total_number_movies = movie_id_set.size();
        this->total_number_users = user_id_set.size();

        /* test */ 
        // cout << "movie to row size: " << this->movie_id_to_row_index.size() << endl;
        // cout << "user to col size: " << this->user_id_to_col_index.size() << endl;
        /* end test */
    }

    void initialize_matrix(void)
    {
        this->rating_matrix = vector<vector<double>>(this->total_number_movies, vector<double>(this->total_number_movies, 0));
        this->is_rated_matrix = vector<vector<bool>>(this->total_number_movies, vector<bool>(this->total_number_users, false));
        this->similarity_matrix = vector<vector<item_sim_pair>>(this->total_number_movies, vector<item_sim_pair>(this->total_number_movies));
    }

};


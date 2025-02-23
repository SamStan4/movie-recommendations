#pragma once

#include "top_list.hpp"

class recommender_wrapper
{
private:
    unordered_map<int, int> movie_id_to_row_index;
    unordered_map<int, int> row_index_to_movie_id;
    unordered_map<int, int> user_id_to_col_index;
    unordered_map<int, int> col_index_to_user_id;
    unordered_map<int, top_list> user_to_movie_reccomendations;
    vector<vector<double>> rating_matrix;
    vector<vector<bool>> is_rated_matrix;
    vector<vector<double>> double_similarity_matrix;
    vector<vector<item_sim_pair>> similarity_matrix;
    vector<double> row_to_euclidean_distance;
    int total_number_movies;
    int total_number_users;


public:

    recommender_wrapper()
    {
        // populates all of the hash tables
        this->populate_maps();

        // initialize matrix
        this->initialize_matrix();

        // import the ratings from the csv file into the marix
        this->populate_rating_matrix();

        // // this will normalize the rating vectors for each movie
        this->center_rating_vectors();

        // // build this for more efficent cosine calculations :)
        this->populate_row_to_euclidean_distance();

        this->populate_similarity_matrix();
        // // this->load_similarity_matrix_from_file();
        // this->export_similarity_matrix();

        this->populate_similariity_pair_matrix();

        this->sort_similarity_matrix();

        this->populate_rating_prediction_matrix();


        // this->export_similarity_pair_matrix();

        // this->load_predicted_scores_from_file();

        this->generate_movie_recommendations();

        this->export_reccomendations();

    }

    double c(int i, int j)
    {
        int k = 0;
        double sum = 0;

        for (k = 0; k < this->total_number_users; ++k)
        {
            sum += this->rating_matrix[i][k] * this->rating_matrix[j][k];
        }

        return sum / (this->row_to_euclidean_distance[i] * this->row_to_euclidean_distance[j]);
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
        for (auto it = movie_id_set.begin(); it != movie_id_set.end(); ++it, ++i)
        {
            this->movie_id_to_row_index[*it] = i; 
            this->row_index_to_movie_id[i] = *it;
        }
        
        i = 0;
        for (auto it = user_id_set.begin(); it != user_id_set.end(); ++it, ++i)
        {
            this->user_id_to_col_index[*it] = i;
            this->col_index_to_user_id[i] = *it;
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
        this->similarity_matrix = vector<vector<item_sim_pair>>(this->total_number_movies, vector<item_sim_pair>(this->total_number_movies, item_sim_pair()));
        this->double_similarity_matrix = vector<vector<double>>(this->total_number_movies, vector<double>(this->total_number_movies, 0));
    }

    void populate_rating_matrix(void)
    {
        ifstream input_stream(RATINGS_FILE_PATH, std::ios::in);
        vector<string> line_split = {};
        string line = "";
        int row = 0, col = 0;
        double rating = 0;

        if (!input_stream.is_open())
        {
            cout << RATINGS_FILE_PATH << " not found" << endl;
            exit(1);
        }

        getline(input_stream, line); // read the file header

        while (getline(input_stream, line))
        {
            split_line(line_split, line, ',');

            row = this->movie_id_to_row_index[stoi(line_split[1])];
            col = this->user_id_to_col_index[stoi(line_split[0])];
            rating = stod(line_split[2]);

            this->rating_matrix[row][col] = rating;

            if (rating > 0)
            {
                this->is_rated_matrix[row][col] = true;
            }
        }

        input_stream.close();
    }

    void center_rating_vectors(void)
    {
        int i = 0, j = 0;
        double k = 0;

        for (i = 0; i < this->total_number_movies; ++i)
        {
            for (j = 0, k = 0; j < this->total_number_users; ++j)
            {
                k += this->rating_matrix[i][j];
            }

            k /= this->total_number_users;

            for (j = 0; j < this->total_number_users; ++j)
            {
                this->rating_matrix[i][j] -= k;
            }
        }
    }

    void populate_row_to_euclidean_distance(void)
    {
        int i = 0, j = 0;
        double k = 0, l = 0;
        this->row_to_euclidean_distance = vector<double>(this->total_number_movies, 0);

        for (i = 0; i < this->total_number_movies; ++i)
        {
            for (j = 0, k = 0; j < this->total_number_users; ++j)
            {
                k += this->rating_matrix[i][j] * this->rating_matrix[i][j];
            }

            this->row_to_euclidean_distance[i] = sqrt(k);

            /* test */

            // if (this->row_to_euclidean_distance[i] == 0)
            // {
            //     cout << "bad" << endl;
            // }

            /* end test */
        }
    }

    void populate_similarity_matrix(void)
    {
        int i = 0, j = 0;
        double k = 0;

        for (i = 0; i < this->total_number_movies; ++i)
        {
            for (j = i; j < this->total_number_movies; ++j)
            {
                k = this->c(i, j); // cosine

                this->double_similarity_matrix[i][j] = k;
                this->double_similarity_matrix[j][i] = k;
            }
        }
    }

    void export_similarity_matrix(void)
    {
        ofstream output_stream("./results.txt", std::ios::out);
        int i = 0, j = 0;

        for (i = 0; i < this->total_number_movies; ++i)
        {
            for (j = 0; j < this->total_number_movies; ++j)
            {
                output_stream << this->double_similarity_matrix[i][j] << ",";
            }

            output_stream << endl;
        }
    }

    void load_similarity_matrix_from_file(void)
    {
        ifstream input_stream("./results.txt", std::ios::in);
        vector<string> line_split;
        string line;
        int i = 0, j = 0;

        for (i = 0; getline(input_stream, line); ++i)
        {
            split_line(line_split, line, ',');

            for (j = 0; j < line_split.size(); ++j)
            {
                this->rating_matrix[i][j] = stod(line_split[j]);
            }
        }

        input_stream.close();
    }

    void populate_similariity_pair_matrix(void)
    {
        int i = 0, j = 0;

        for (i = 0; i < this->total_number_movies; ++i)
        {
            for (j = 0; j < this->total_number_movies; ++j)
            {
                this->similarity_matrix[i][j].set_both_attributes(this->row_index_to_movie_id[j], this->double_similarity_matrix[i][j]);
            }
        }
    }

    void export_similarity_pair_matrix(void)
    {
        ofstream output_stream("./results.txt", std::ios::out);
        int i = 0, j = 0;

        for (i = 0; i < this->total_number_movies; ++i)
        {
            output_stream << this->row_index_to_movie_id[i] << "-";

            for (j = 0; j < this->total_number_movies; ++j)
            {
                output_stream << "|" << this->similarity_matrix[i][j].get_item_id() << "," << this->similarity_matrix[i][j].get_sim_score() << "|";
            }

            output_stream << endl;
        }
    }

    void sort_similarity_matrix(void)
    {
        int i = 0;
        for (i = 0; i < this->total_number_movies; ++i)
        {
            sort(this->similarity_matrix[i].begin(), this->similarity_matrix[i].end());
            reverse(this->similarity_matrix[i].begin(), this->similarity_matrix[i].end());
        }
    }

    void populate_rating_prediction_matrix(void)
    {
        int i = 0, j = 0;

        cout << "here" << endl;

        for (i = 0; i < this->total_number_movies; ++i)
        {
            for (j = 0; j < this->total_number_users; ++j)
            {
                if (!this->is_rated_matrix[i][j])
                {
                    this->rating_matrix[i][j] = this->make_rating_prediction(i, j);
                }
            }
        }
    }

    double make_rating_prediction(int movie_idx, int user_idx)
    {
        // get movies similar to the current movie, that the user has rated

        int i = 0, j = 0;
        double numerator = 0, denominator = 0;
        top_list user_top_similar;

        for (i = 0, j = 0; (j < 5) && (i < this->total_number_movies); ++i)
        {
            if (this->is_rated_matrix[this->movie_id_to_row_index[this->similarity_matrix[movie_idx][i].get_item_id()]][user_idx])
            {
                ++j;
                numerator += this->similarity_matrix[movie_idx][i].get_sim_score() * this->rating_matrix[this->movie_id_to_row_index[this->similarity_matrix[movie_idx][i].get_item_id()]][user_idx];
                denominator += this->similarity_matrix[movie_idx][i].get_sim_score();
            }
        }

        return max(numerator / denominator, 1.0);
    }

    void export_rating_matrix(void)
    {
        ofstream output_stream("./AAA.csv", std::ios::out);
        int i = 0, j = 0;
        
        output_stream << ",";

        for (i = 0; i < this->total_number_users; ++i)
        {
            output_stream << this->col_index_to_user_id[i] << ",";
        }

        output_stream << endl;

        for (i = 0; i < this->total_number_movies; ++i)
        {
            output_stream << this->row_index_to_movie_id[i] << ",";

            for (j = 0; j < this->total_number_users; ++j)
            {
                output_stream << this->rating_matrix[i][j] << ",";
            }

            output_stream << endl;
        }

        output_stream.close();
    }

    void load_predicted_scores_from_file(void)
    {
        ifstream input_stream("./AAA.csv", std::ios::in);
        string line;
        vector<string> line_split, user_header;
        int i = 0, movie_idx, user_idx;

        getline(input_stream, line);

        split_line(user_header, line, ',');

        while (getline(input_stream, line))
        {
            split_line(line_split, line, ',');

            movie_idx = this->movie_id_to_row_index[stoi(line_split[0])];

            for (i = 1; i < line_split.size(); ++i)
            {
                user_idx = this->user_id_to_col_index[stoi(user_header[i-1])];

                this->rating_matrix[movie_idx][user_idx] = stod(line_split[i]);
            }
        }

    }

    void generate_movie_recommendations(void)
    {
        int i = 0, j = 0;

        cout << "here" << endl;

        for (i = 0; i < this->total_number_movies; ++i)
        {
            for (j = 0; j < this->total_number_users; ++j)
            {
                this->user_to_movie_reccomendations[this->col_index_to_user_id[j]]._add_item(this->row_index_to_movie_id[i], this->rating_matrix[i][j]);
            }
        }
    }

    void export_reccomendations(void)
    {
        ofstream output_stream("reccomendations.txt", std::ios::out);
        vector<item_sim_pair> temp;
        int i = 0;

        for (auto it = this->user_to_movie_reccomendations.begin(); it != this->user_to_movie_reccomendations.end(); ++it)
        {
            temp = it->second.get_list();

            output_stream << it->first;

            for (i = 0; i < temp.size(); ++i)
            {
                output_stream << " " << temp[i].get_item_id();
            }

            output_stream << endl;
        }

        output_stream.close();
    }
};


#pragma once

#include "imports.hpp"

class item_sim_pair
{
private:

    int item_id;
    double sim_score;

public:

    item_sim_pair()
    {
        this->item_id = 0;
        this->sim_score = 0;
    }

    void set_item_id(int new_item_id)
    {
        this->item_id = new_item_id;
    }

    void set_sim_score(double new_sim_score)
    {
        this->sim_score = new_sim_score;
    }

    void set_both_attributes(int new_item_id, double new_sim_score)
    {
        this->item_id = new_item_id;
        this->sim_score = new_sim_score;
    }

    int get_item_id(void)
    {
        return this->item_id;
    }

    double get_sim_score(void)
    {
        return this->sim_score;
    }

    bool operator<(item_sim_pair& other)
    {
        double other_sim_score = other.get_sim_score();

        if (this->sim_score != other_sim_score)
        {
            return this->sim_score < other_sim_score;
        }

        return this->item_id > other.get_item_id();
    }

    bool operator>(item_sim_pair& other)
    {
        double other_sim_score = other.get_sim_score();

        if (this->sim_score != other_sim_score)
        {
            return this->sim_score > other_sim_score;
        }

        return this->item_id < other.get_item_id();
    }

    bool operator==(item_sim_pair& other)
    {
        return (this->sim_score == other.get_sim_score()) && (this->item_id == other.get_item_id());
    }

};
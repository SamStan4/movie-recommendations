#pragma once

#include "item_sim_pair.hpp"

class top_list
{
private:

    int list_size;
    vector<item_sim_pair> list;

public:

    top_list()
    {
        this->list_size = GIVEN_NEIGHBORHOOD_SIZE;
    }

    void add_item(item_sim_pair& new_item)
    {
        if (this->list.size() < this->list_size)
        {
            this->list.push_back(new_item);
            sort(this->list.begin(), this->list.end());
        }
        else if (this->list[0] < new_item)
        {
            this->list[0] = new_item;
            sort(this->list.begin(), this->list.end());
        }
    }

    vector<item_sim_pair> get_list(void)
    {
        return this->list;
    }
};
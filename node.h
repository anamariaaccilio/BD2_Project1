//
// Created by ACER on 03/09/2023.
//

#ifndef BD2_PROJECT1_NODE_H
#define BD2_PROJECT1_NODE_H

#include <iostream>
#include <string>
#include <iomanip>
#include <ostream>
#include <sstream>


template <typename T>
struct NodeBT {
    T data;
    NodeBT<T> *left;
    NodeBT<T> *right;
    NodeBT<T> *parent;
    int height;

    NodeBT(T data) {
        this->data = data;
        this->left = nullptr;
        this->right = nullptr;
        this->parent = nullptr;
        this->height = 0;
    }

    NodeBT(T data, NodeBT<T> *left, NodeBT<T> *right, NodeBT<T> *parent) {
        this->data = data;
        this->left = left;
        this->right = right;
        this->parent = parent;
        this->height = 0;
    }

    NodeBT(T data, NodeBT<T> *left, NodeBT<T> *right, NodeBT<T> *parent, int height) {
        this->data = data;
        this->left = left;
        this->right = right;
        this->parent = parent;
        this->height = height;
    }

    std::string toString() {
        std::ostringstream oss;
        oss << std::setw(4) << std::setfill('0') << data;
        return oss.str();
    }





};


#endif //BD2_PROJECT1_NODE_H

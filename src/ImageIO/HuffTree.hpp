/*
 * This file is part of RawDev;
 * see <https://github.com/petrk23/RawDev>.
 *
 * Copyright (C) 2020-2025 Petr Krajník
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cinttypes>
#include <ostream>

class HuffTree
{
    /// <summary>
    /// Huffman tree node structure
    /// </summary>
    /// <remarks>
    /// For speed optimization there is no inheritance
    /// virtual functions and other stuff that slowed down.
    /// </remarks>
    struct Node {
        Node* left, * right;
        int value; // Leaf value
        bool leaf; // Leaf indicator

        ~Node(); // Destructor
    };

    /// <summary>
    /// Huffman tree root node
    /// </summary>
    Node* m_root;

public:
    class iterator;
    iterator begin() const;
    static const iterator end;

public:
    HuffTree(); ~HuffTree();
    HuffTree& operator() (uint8_t* codeCounts, uint8_t* values);
    friend std::ostream &operator<< (std::ostream &os, const HuffTree &ht);

private:
    void insert(int code, int codelen, int val);
    void destroy(); // Destroy and free tree memory
    int  genSizeTable(uint8_t* codeCounts, unsigned int* sizes);
    void genCodeTable(unsigned int* sizes, unsigned int* codes);
};

////////////////////////////////////////////////////////////////////////////////

class HuffTree::iterator
{
    Node* m_node;

public:
    iterator(Node* start);
    bool operator==(const iterator& it) const;
    bool operator!=(const iterator& it) const;
    void next(int bit);
    bool leaf(int& val) const;
};

inline HuffTree::iterator::iterator(Node* start)
    : m_node(start)
{}

inline bool HuffTree::iterator::operator==(const iterator& it) const
{
    return m_node == it.m_node;
}

inline bool HuffTree::iterator::operator!=(const iterator& it) const
{
    return m_node != it.m_node;
}

inline void HuffTree::iterator::next(int bit)
{
    if (m_node != nullptr && m_node->leaf == false) {
        if (bit == 0)
            m_node = m_node->left;
        else
            m_node = m_node->right;
    }
    return;
}

inline bool HuffTree::iterator::leaf(int& val) const
{
    if (m_node != nullptr && m_node->leaf == true) {
        val = m_node->value;
        return true;
    }
    return false;
}

inline HuffTree::HuffTree()
    : m_root(nullptr)
{}

inline HuffTree::~HuffTree()
{
    destroy();
    return;
}

inline HuffTree::iterator HuffTree::begin() const
{
    return iterator(m_root);
}

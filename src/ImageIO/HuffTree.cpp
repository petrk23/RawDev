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

#include "HuffTree.hpp"

#include <vector>
#include <cassert>

/// <summary>
/// End iterator constant initialization
/// </summary>
const HuffTree::iterator HuffTree::end = iterator(nullptr);

/// <summary>
/// Reconstruct new huffman tree
/// </summary>
/// <param name="codeCounts">Code count array</param>
/// <param name="values">Values associated with codes</param>
/// <returns>Reference to the created huffman tree</returns>
HuffTree& HuffTree::operator() (uint8_t* codeCounts, uint8_t* values)
{
    unsigned int sizes[16], codes[16];

    // Destroy previous tree
    destroy();

    // Generate codes
    const int lastk = genSizeTable(codeCounts, sizes);
    genCodeTable(sizes, codes);

    // Generate huffman tree
    for (int c = 0; c < lastk; c++) {
        insert(codes[c], sizes[c], values[c]);
    }
    return *this;
}

/// <summary>
/// Inserting a new code into the tree
/// </summary>
/// <param name="code">Code to be inserted</param>
/// <param name="codelen">Lenght of that code</param>
/// <param name="val">Value associated with that code</param>
void HuffTree::insert(int code, int codelen, int val)
{
    Node** root = &m_root, * node;

    for (; codelen > 0; codelen--)
    {
        node = *root; // Load root pointer

        // Internal node don't exist
        if (node == nullptr) {
            node = new Node();
            node->left = nullptr;
            node->right = nullptr;
            node->leaf = false;
            *root = node;
        }
        // Direction of inserting
        if ((code & (1 << (codelen - 1))) == 0) {
            root = &(node->left);
        }
        else {
            root = &(node->right);
        }
    }
    assert(codelen == 0);
    node = new Node(); // Leaf node
    node->leaf = true;
    node->value = val;
    *root = node;
    return;
}

/// <summary>
/// Destroying huffman tree
/// </summary>
void HuffTree::destroy()
{
    delete m_root;
    m_root = nullptr;
    return;
}

/// <summary>
/// Generate huffman code sizes
/// </summary>
/// <param name="codeCounts">
/// Gives codeword count of len 'i', where 'i' is
/// the count of bits of that codeword numbered
/// from 1 to 16. So index0 means codeword count
/// with 1bit and not 0bit.
/// </param>
/// <param name="sizes">
/// Resulting code size array
/// </param>
int HuffTree::genSizeTable(uint8_t* codeCounts, unsigned int* sizes)
{
    int k = 0; // Final codeword count

    for (int c = 1; c <= 16; c++)
    {
        int len = codeCounts[c - 1]; // Value
        for (int j = 1; j <= len; j++) {
            sizes[k++] = c;
        }
    }
    sizes[k] = 0;
    return k;
}

/// <summary>
/// Generatres codes for the huffman tree
/// </summary>
/// <param name="huffsize">Huffman sizes for each code</param>
/// <param name="huffcode">Resulting array of codes</param>
void HuffTree::genCodeTable(unsigned int* huffsize, unsigned int* huffcode)
{
    unsigned int code = 0, si = huffsize[0];

    for (int k = 0;;)
    {
        // Consecutively numbered when size equals
        for (; huffsize[k] == si; code++, k++) {
            huffcode[k] = code;
        }
        // With zero we are done (zero terminated)
        if (huffsize[k] == 0) {
            break;
        }
        // Prepare for the next codeword
        else for (; huffsize[k] != si; si++) {
            code <<= 1;
        }
    }
    return;
}

/// <summary>
/// Print huffman tree information
/// </summary>
/// <param name="os">Output stream for print</param>
/// <param name="ht">Reference for a huffman tree</param>
/// <returns>Output stream for future usage</returns>
/// <remarks>
/// We dont need it, so no implementation yet.
/// </remarks>
std::ostream &operator<<(std::ostream &os, const HuffTree &)
{
    return os;
}

/// <summary>
/// Recursive node destructor
/// </summary>
HuffTree::Node::~Node()
{
    delete left;
    left = nullptr;
    delete right;
    right = nullptr;
    return;
}

/* 
 * File:   lazy_string.cpp
 * Author: 0xC0deBabe <iam@kostya.sexy>
 * 
 * Created on 14 мая 2016 г., 11:05
 */

#include "lazy_string.h"
#include <iostream>

using namespace std;

lazy_string::lazy_string() {
    this->present = string();
    this->start = this->sizevar = 0;
}

lazy_string::lazy_string(const string &string) {
    this->present = string;
    this->start = 0;
    this->sizevar = string.size();
}

lazy_string::lazy_string(const lazy_string &ls, size_t start, size_t size) {
    this->present = ls.present;
    this->start = start;
    this->sizevar = size;
}

size_t lazy_string::size() const {
    return this->sizevar;
}

size_t lazy_string::length() const {
    return this->sizevar;
}

const char& lazy_string::at(size_t pos) const {
    return this->present[this->start + pos];
}

const char& lazy_string::operator[](size_t pos) const {
    return at(pos);
}

istream& operator>>(istream &is, lazy_string &ls) {
    is >> ls.present;
    ls.start = 0;
    ls.sizevar = ls.present.size();
    return is;
}

ostream& operator<<(ostream &os, lazy_string &ls) {
    for(size_t i = ls.start, j = 0; j < ls.sizevar; ++i, ++j)
        os << ls[i];
    return os;
}
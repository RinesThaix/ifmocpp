/* 
 * File:   lazy_string.cpp
 * Author: 0xC0deBabe <iam@kostya.sexy>
 * 
 * Created on 14 мая 2016 г., 11:05
 */

#include "lazy_string.h"
#include <iostream>
#include <stdexcept>
#include <pthread.h>

using namespace std;

lazy_string::lazy_string() {
    lock();
    this->present = string();
    this->start = this->sizevar = 0;
    unlock();
}

lazy_string::lazy_string(const string &string) {
    lock();
    this->present = string;
    this->start = 0;
    this->sizevar = string.size();
    unlock();
}

lazy_string::lazy_string(const lazy_string &ls, size_t start, size_t size) {
    lock();
    this->present = ls.present;
    this->start = start;
    this->sizevar = size;
    unlock();
}

void lazy_string::lock() const {
    pthread_mutex_lock(&this->sync);
}

void lazy_string::unlock() const {
    pthread_mutex_unlock(&this->sync);
}

size_t lazy_string::size() const {
    lock();
    size_t result = this->sizevar;
    unlock();
    return result;
}

size_t lazy_string::length() const {
    return size();
}

const char& lazy_string::at(size_t pos) const {
    lock();
    const char& result = this->present[this->start + pos];
    unlock();
    return result;
}

const char& lazy_string::operator[](size_t pos) const {
    return at(pos);
}

const char& lazy_string::setCharAt(size_t pos, const char &value) {
    if(pos > size())
        throw out_of_range("Can not set character at specified index (lazy_string)");
    lock();
    const string svalue(1, value);
    this->present.replace(this->start + pos, 1, svalue);
    unlock();
    return value;
}

lazy_string lazy_string::substr(size_t pos, size_t len) {
    if(pos > size())
        throw out_of_range("Can not take substring of given length from the given starting position (lazy_string)");
    lock();
    lazy_string result = lazy_string(this->present, this->start + pos, pos + len > size() ? (size() - pos) : len);
    unlock();
    return result;
}

istream& operator>>(istream &is, lazy_string &ls) {
    ls.lock();
    is >> ls.present;
    ls.start = 0;
    ls.sizevar = ls.present.size();
    ls.unlock();
    return is;
}

ostream& operator<<(ostream &os, lazy_string &ls) {
    ls.lock();
        for(size_t j = 0; j < ls.size(); ++j)
            os << ls.present[ls.start + j];
    ls.unlock();
    return os;
}
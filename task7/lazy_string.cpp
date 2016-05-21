/* 
 * File:   lazy_string.cpp
 * Author: 0xC0deBabe <iam@kostya.sexy>
 * 
 * Created on 14 мая 2016 г., 11:05
 */

#include "lazy_string.h"
#include <iostream>
#include <stdexcept>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>

using namespace std;

ReadWriteLock::ReadWriteLock() : readers(0), locked(false), rec(0) {}

inline void ReadWriteLock::readLock() const {
    if(this->threadId == this_thread::get_id()) {
        ++this->readers;
        return;
    }
    while(locked)
        this_thread::yield();
    ++this->readers;
}

inline void ReadWriteLock::readUnlock() const {
    --this->readers;
}

inline void ReadWriteLock::writeLock() {
    thread::id id = this_thread::get_id();
    if(id == this->threadId) {
        ++this->rec;
        return;
    }
    while(atomic_exchange_explicit(&this->locked, true, memory_order_acquire))
        this_thread::yield();
    while(this->readers > 0)
        this_thread::yield();
    this->threadId = id;
    ++this->rec;
}

inline void ReadWriteLock::writeUnlock() {
    if(--this->rec == 0) {
        this->threadId = thread::id();
        atomic_store_explicit(&this->locked, false, memory_order_release);
    }
}

lazy_string::lazy_string() {
    this->present = make_shared<std::string>(string());
    this->start = this->sizevar = 0;
}

lazy_string::lazy_string(const string &string) {
    this->present = make_shared<std::string>(string);
    this->start = 0;
    this->sizevar = string.size();
}

lazy_string::lazy_string(const lazy_string &ls) {
    writeLock();
    ls.writeLock();
    this->present = ls.present;
    this->start = ls.start;
    this->sizevar = ls.sizevar;
    ls.writeUnlock();
    writeUnlock();
}

void lazy_string::writeLock() const {
    this->lock.writeLock();
}

void lazy_string::writeUnlock() const {
    this->lock.writeUnlock();
}

void lazy_string::readLock() const {
    this->lock.readLock();
}

void lazy_string::readUnlock() const {
    this->lock.readUnlock();
}

size_t lazy_string::size() const {
    readLock();
    size_t result = this->sizevar;
    readUnlock();
    return result;
}

size_t lazy_string::length() const {
    return size();
}

const char& lazy_string::at(size_t pos) const {
    readLock();
    const char& result = (*this->present)[this->start + pos];
    readUnlock();
    return result;
}

const char& lazy_string::operator[](size_t pos) const {
    return at(pos);
}

const char& lazy_string::setCharAt(size_t pos, const char &value) {
    if(pos > size())
        throw out_of_range("Can not set character at specified index (lazy_string)");
    writeLock();
    const string svalue(1, value);
    (*this->present).replace(this->start + pos, 1, svalue);
    writeUnlock();
    return value;
}

lazy_string lazy_string::substr(size_t pos, size_t len) {
    if(pos > size())
        throw out_of_range("Can not take substring of given length from the given starting position (lazy_string)");
    readLock();
    lazy_string result;
    result.present = this->present;
    result.start = this->start + pos;
    result.sizevar = pos + len > this->sizevar ? this->sizevar - pos : len;
    readUnlock();
    return result;
}

istream& operator>>(istream &is, lazy_string &ls) {
    ls.writeLock();
    is >> *ls.present;
    ls.start = 0;
    ls.sizevar = (*ls.present).size();
    ls.writeUnlock();
    return is;
}

ostream& operator<<(ostream &os, lazy_string &ls) {
    ls.readLock();
        for(size_t j = 0; j < ls.size(); ++j)
            os << (*ls.present)[ls.start + j];
    ls.readUnlock();
    return os;
}
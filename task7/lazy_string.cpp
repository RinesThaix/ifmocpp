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

ReadWriteLock::ReadWriteLock(const ReadWriteLock &lock) :
    readers((int) lock.readers), locked((bool) lock.locked), rec((int) lock.rec) {}

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
    while(atomic_exchange_explicit(&locked, true, memory_order_acquire))
        this_thread::yield();
    while(this->readers > 0)
        this_thread::yield();
    this->threadId = id;
    ++this->rec;
}

inline void ReadWriteLock::writeUnlock() {
    if(--this->rec == 0) {
        this->threadId = thread::id();
        atomic_store_explicit(&locked, false, memory_order_release);
    }
}

lazy_string::lazy_string() {
    this->present = make_shared<std::string>(string());
    this->start = this->sizevar = 0;
    this->lock = make_shared<ReadWriteLock>(ReadWriteLock());
}

lazy_string::lazy_string(const string &string) {
    this->present = make_shared<std::string>(string);
    this->start = 0;
    this->sizevar = string.size();
    this->lock = make_shared<ReadWriteLock>(ReadWriteLock());
}

lazy_string::lazy_string(const lazy_string &ls) {
    ReadWriteLock &current = (*this->lock);
    current.writeLock();
    (*ls.lock).writeLock();
    this->present = ls.present;
    this->start = ls.start;
    this->sizevar = ls.sizevar;
    this->lock = ls.lock;
    (*ls.lock).writeUnlock();
    current.writeUnlock();
}

lazy_string::char_reference::char_reference(lazy_string *ls, size_t index) : ls(ls), index(index) { }

size_t lazy_string::size() const {
    lock->readLock();
    size_t result = this->sizevar;
    lock->readUnlock();
    return result;
}

size_t lazy_string::length() const {
    return size();
}

const char& lazy_string::at(size_t pos) const {
    lock->readLock();
    const char& result = (*this->present)[this->start + pos];
    lock->readUnlock();
    return result;
}

lazy_string::char_reference lazy_string::operator[](size_t pos) {
    return char_reference(this, pos);
}

const char& lazy_string::operator[](size_t pos) const {
    return at(pos);
}

lazy_string::char_reference &lazy_string::char_reference::operator=(char value) {
    ReadWriteLock &current = (*this->ls->lock);
    current.writeLock();
    bool updating = false;
    if(this->ls->present.use_count() > 1) {
        this->ls->present = make_shared<std::string>(this->ls->present->substr(this->ls->start, this->ls->sizevar));
        this->ls->start = 0;
        ReadWriteLock newLock;
        newLock.writeLock();
        this->ls->lock = make_shared<ReadWriteLock>(newLock);
        updating = true;
    }
    const string svalue(1, value);
    (*this->ls->present).replace(this->ls->start + this->index, 1, svalue);
    current.writeUnlock();
    if(updating)
        this->ls->lock->writeUnlock();
    return *this;
}

lazy_string lazy_string::substr(size_t pos, size_t len) {
    if(pos > size())
        throw out_of_range("Can not take substring of given length from the given starting position (lazy_string)");
    lock->readLock();
    lazy_string result;
    result.present = this->present;
    result.start = this->start + pos;
    result.sizevar = pos + len > this->sizevar ? this->sizevar - pos : len;
    lock->readUnlock();
    return result;
}

istream& operator>>(istream &is, lazy_string &ls) {
    ls.lock->writeLock();
    is >> *ls.present;
    ls.start = 0;
    ls.sizevar = (*ls.present).size();
    ls.lock->writeUnlock();
    return is;
}

ostream& operator<<(ostream &os, lazy_string &ls) {
    ls.lock->readLock();
        for(size_t j = 0; j < ls.size(); ++j)
            os << (*ls.present)[ls.start + j];
    ls.lock->readUnlock();
    return os;
}
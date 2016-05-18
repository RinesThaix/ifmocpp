/* 
 * File:   lazy_string.h
 * Author: 0xC0deBabe <iam@kostya.sexy>
 *
 * Created on 14 мая 2016 г., 11:05
 */

#ifndef LAZY_STRING_H
#define	LAZY_STRING_H

#include <string>
#include <istream>
#include <ostream>

using namespace std;

class lazy_string {
    
private:
    size_t start, sizevar;
    string present;
    lazy_string(const lazy_string &parent, size_t start, size_t size);
    
public:
    
    /**
     * Creates empty lazy_string.
     */
    lazy_string();
    
    /**
     * Creates new lazy_string from the given std::string.
     * @return lazy_string instance.
     */
    lazy_string(const string &string);
    
    /**
     * Returns string's length (size).
     * @return string's length (size).
     */
    size_t size() const;
    
    /**
     * Returns string's length (size).
     * @return string's length (size).
     */
    size_t length() const;
    
    /**
     * Creates std::string using COW method from this lazy_string.
     * @return std::string instance.
     */
    operator std::string();
    
    /**
     * Returns character at specified index.
     * @param character's index.
     * @return character at specified index.
     */
    const char& at(size_t pos) const;
    
    /**
     * Returns character at specified index.
     * @param character's index.
     * @return character at specified index.
     */
    const char& operator[](size_t pos) const;
    
    /**
     * Returns a newly constructed lazy_string object with its value initialized to a copy of a substring of this object.
     * @param pos position of the first character to be copied as a substring.
     * @param len number of characters to include in the substring
     * (if the string is shorter, as many characters as possible are used).
     * @return A lazy_string object with a substring of this object.
     * @throws std::out_of_range whether pos > size()
     */
    lazy_string substr(size_t pos = 0, size_t len = string::npos);
    
    /**
     * Extracts a lazy_string from the input stream is, storing the sequence in ls,
     * which is overwritten (the previous value of ls is replaced).
     * @param is input stream.
     * @param ls lazy_string.
     * @return the same as parameter is.
     */
    friend istream& operator>>(istream &is, lazy_string &ls);
    
    /**
     * Inserts the sequence of characters that conforms value of ls into os.
     * @param os output stream.
     * @param ls lazy_string.
     * @return the same as parameter os.
     */
    friend ostream& operator<<(ostream &os, lazy_string &ls);
    
};

#endif	/* LAZY_STRING_H */


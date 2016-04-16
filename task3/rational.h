/* 
 * File:   rational.h
 * Author: 0xC0deBabe <iam@kostya.sexy>
 *
 * Created on 16 апреля 2016 г., 13:31
 */

#ifndef RATIONAL_H
#define	RATIONAL_H

class rational {
    
private:
    int num;
    int denom;
    const int gcd(int a, int b);
    
public:
    rational(int n, int d);
    rational(int r);
    
    const int getNum();
    const int getDenom();
    
    const rational operator +(const rational& second);
    const rational operator -(const rational& second);
    const rational operator *(const rational& second);
    const rational operator /(const rational& second);
    
};

#endif	/* RATIONAL_H */


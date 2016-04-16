#include "rational.h"

int rational::gcd(int a, int b) const {
    if(a == 0)
        return b;
    return gcd(b % a, a);
}

rational::rational(int n, int d) {
    int g = gcd(n, d);
    rational::num = n / g;
    rational::denom = d / g;
}

rational::rational(int num) {
    rational::num = num;
    rational::denom = 1;
}

int rational::getNum() const {
    return rational::num;
}

int rational::getDenom() const {
    return rational::denom;
}

rational rational::operator +(const rational& second) const {
    int n1 = getNum(), d1 = getDenom(), n2 = second.getNum(), d2 = second.getDenom();
    int denom = d1 * d2 / gcd(d1, d2);
    int num = n1 * denom / d1 + n2 * denom / d2;
    return rational(num, denom);
}

rational rational::operator -(const rational& second) const {
    int n1 = getNum(), d1 = getDenom(), n2 = second.getNum(), d2 = second.getDenom();
    int denom = d1 * d2 / gcd(d1, d2);
    int num = n1 * denom / d1 - n2 * denom / d2;
    return rational(num, denom);
}

rational rational::operator *(const rational& second) const {
    return rational(getNum() * second.getNum(), getDenom() * second.getDenom());
}

rational rational::operator /(const rational& second) const {
    return rational(getNum() * second.getDenom(), getDenom() * second.getNum());
}
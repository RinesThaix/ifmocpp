#include "rational.h"

const int rational::gcd(int a, int b) {
    if(a == 0)
        return b;
    return gcd(b % a, a);
}

rational::rational(int n, int d) {
    int gcd = gcd(n, d);
    rational::num = n / gcd;
    rational::denom = d / gcd;
}

rational::rational(int r) {
    rational(r, 1);
}

const int rational::getNum() {
    return rational::num;
}

const int rational::getDenom() {
    return rational::denom;
}

const rational rational::operator +(const rational& second) {
    int n1 = getNum(), d1 = getDenom(), n2 = second.getNum(), d2 = second.getDenom();
    int denom = d1 * d2 / gcd(d1, d2);
    int nom = (n1 / d1 + n2 / d2) * denom;
    return rational(nom, denom);
}

const rational rational::operator -(const rational& second) {
    int n1 = getNum(), d1 = getDenom(), n2 = second.getNum(), d2 = second.getDenom();
    int denom = d1 * d2 / gcd(d1, d2);
    int nom = (n1 / d1 - n2 / d2) * denom;
    return rational(nom, denom);
}

const rational rational::operator *(const rational& second) {
    return rational(getNum() * second.getNum(), getDenom() * second.getDenom());
}

const rational rational::operator /(const rational& second) {
    return rational(getNum() * second.getDenom(), getDenom() * second.getNum());
}
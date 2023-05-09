#include "vec2.h"
#include <algorithm>
#include <cmath>

Coord::Coord(int x,int y)
:   x(x), y(y){}

Coord::Coord(int i)
:   x(i), y(i){}

Coord::Coord()
:   x(0), y(0){}

Coord Coord::operator+(const Coord& in) const {
    return Coord(this->x+in.x, this->y+in.y);
}
Coord::operator Vec2() const {
    return Vec2(x,y);
}
Coord Coord::operator- () const {
    return Coord(-x, -y);
}
bool Coord::operator== (const Coord& in) const {
    return this->x==in.x && this->y==in.y;
}
int Coord::dot(const Coord& other) const {
    return x*other.x + y*other.y;
}



const Vec2 operator+(double lhs, const Vec2& rhs) { return rhs + lhs; }
const Vec2 operator-(double lhs, const Vec2& rhs) { return (-rhs) + lhs; }
const Vec2 operator*(double lhs, const Vec2& rhs) { return rhs * lhs; }
const Vec2 operator/(double lhs, const Vec2& rhs) { return Vec2(lhs/rhs.x, lhs/rhs.y); };

const Vec2 Vec2::operator- (double in) const {
    return Vec2(x-in, y-in);
}

Vec2::Vec2(double x, double y)
:   x(x), y(y) {}
Vec2::Vec2(double i)
:   x(i), y(i) {}
Vec2::Vec2()
:   x(0), y(0) {}

const Vec2 Vec2::operator + (const Vec2& in) const {
    return Vec2(this->x+in.x, this->y+in.y);
}
const Vec2 Vec2::operator + (double in) const {
    return Vec2(this->x+in, this->y+in);
}
const Vec2 Vec2::operator * (double in) const {
    return Vec2(this->x*in, this->y*in);
}
const Vec2 Vec2::operator - () const {
    return Vec2(-this->x, -this->y);
}
const Vec2 Vec2::project(const Vec2& in) const {
    //a * (a dot b) / (|a|^2)
    return *this * (this->dot(in)/(this->mag()*this->mag()));
}
const Vec2 Vec2::operator - (const Vec2& in) const {
    return (*this)+(-in);
}
double Vec2::magNoRoot() const {
    return x*x + y*y;
}
double Vec2::mag() const {
    return std::sqrt(magNoRoot());
}
double Vec2::getDistanceNoRoot( const Vec2& in ) const {
    return (in.x-this->x)*(in.x-this->x) + (in.y-this->y)*(in.y-this->y);
}
double Vec2::getDistance( const Vec2& in) const {
    return std::sqrt(getDistanceNoRoot(in));
}
double Vec2::getDistanceManhattan(const Vec2& in) const {
    return std::abs(this->x - in.x) + std::abs(this->y - in.y);
}
bool Vec2::operator== (const Vec2& in) const {
    return this->x==in.x && this->y==in.y;
}
bool Vec2::operator!= (const Vec2& in) const {
    return !(*this==in);
}
const Vec2 Vec2::rotateLeft() const {
    return Vec2(-this->y, this->x);
}
const Vec2 Vec2::rotateRight() const {
    return -rotateLeft();
}
const Vec2 Vec2::rotateLeft45() const {
    return rotateLeft() + *this;
}
const Vec2 Vec2::rotateRight45() const {
    return rotateRight() + *this;
}
const Vec2 Vec2::operator/ (double in) const {
    return Vec2(x/in, y/in);
}

double Vec2::magManhattan() const {
    return std::max(std::abs(x), std::abs(y));
}
const Vec2 Vec2::normalizeManhattan() const {
    return *this/magManhattan();
}
Vec2::operator Coord() const {
    return Coord(std::lround(x), std::lround(y));
}

const Vec2 Vec2::normalize() const {
    return *this / mag();
}

std::vector<Vec2> Vec2::getNeighbors() const {
    Vec2 center = this->normalizeManhattan();

    Vec2 neighbors[] = {center + Vec2(-1, -1), center + Vec2(0, -1), center + Vec2(1, -1),
                        center + Vec2(-1,  0), /*ignore center pos*/ center + Vec2(1,  0),
                        center + Vec2(-1,  1), center + Vec2(0,  1), center + Vec2(1,  1)};

    return std::vector<Vec2>(neighbors, neighbors + 8);
}

std::vector<Coord> Coord::getNeighbors() const {
    Coord center = *this;

    Coord neighbors[] = {center + Coord(-1, -1), center + Coord(0, -1), center + Coord(1, -1),
                        center + Coord(-1,  0), /*ignore center pos*/ center + Coord(1,  0),
                        center + Coord(-1,  1), center + Coord(0,  1), center + Coord(1,  1)};

    return std::vector<Coord>(neighbors, neighbors + 8);
}

Vec2 Coord::toVec2() const {
    return (Vec2) *this;
}

double Vec2::dot(const Vec2& in) const {
    return this->x*in.x + this->y*in.y;
}

const Vec2 Vec2::operator/(const Vec2& in) const {
    return Vec2(x / in.x, y / in.y);
}

const Vec2 Vec2::operator*(const Vec2& in) const {
    return Vec2(x * in.x, y * in.y);
}




const Vec2 Vec2::piecewiseMin(const Vec2& other) const {
    return Vec2(std::min(x, other.x), std::min(y, other.y));
}

const Vec2 Vec2::piecewiseMax(const Vec2& other) const {
    return Vec2(std::max(x, other.x), std::max(y, other.y));
}

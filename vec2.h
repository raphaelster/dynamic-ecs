#ifndef VEC2_H
#define VEC2_H

#include <functional>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

class Vec2;

struct Coord {
    int x,y;

    Coord(int x,int y);
    explicit Coord(int i);
    Coord();

    Coord operator + (const Coord& in) const;
    Coord operator - () const;
    operator Vec2() const;
    bool operator==(const Coord& in) const;
    std::vector<Coord> getNeighbors() const;
    Vec2 toVec2() const;
    int dot(const Coord& other) const;

    friend std::ostream& operator<<(std::ostream& s, const Coord& c) {
        s << "C("<<c.x<<", "<<c.y<<")";
        return s;
    }
};

struct Vec2 {
    double x, y;

    ///for some reason, g++ doesn't like it when these ctors are constexpr
    Vec2();
    explicit Vec2(double i);
    Vec2(double x, double y);

    const Vec2 operator + (const Vec2& in) const;
    const Vec2 operator + (double in) const;
    const Vec2 operator * (double in) const;
    const Vec2 operator * (const Vec2& in) const;
    const Vec2 operator / (double in) const;
    const Vec2 operator / (const Vec2& in) const;
    const Vec2 operator- () const;
    const Vec2 operator- (double) const;
    const Vec2 operator- (const Vec2& in) const;
    const Vec2 project(const Vec2& in) const;
    const Vec2 rotateLeft () const;
    const Vec2 rotateRight() const;
    const Vec2 rotateLeft45() const;
    const Vec2 rotateRight45() const;
    bool operator== (const Vec2& in) const;
    bool operator!= (const Vec2& in) const;
    double getDistance(const Vec2& in) const;
    double getDistanceNoRoot(const Vec2& in) const;
    double mag() const;
    double magNoRoot() const;
    double dot(const Vec2& in) const;
    const Vec2 normalizeManhattan() const;
    double magManhattan() const;
    double getDistanceManhattan(const Vec2& in) const;
    std::vector<Vec2> getNeighbors() const;
    explicit operator Coord() const;
    const Vec2 normalize() const;

    const Vec2 piecewiseMin(const Vec2& other) const;
    const Vec2 piecewiseMax(const Vec2& other) const;

    const glm::vec2 toGlmVec2() const { return glm::vec2(x, y); }

    friend std::ostream& operator<< (std::ostream& s, const Vec2& c) {
        s << "V("<<c.x<<", "<<c.y<<")";
        return s;
    }
};

extern const Vec2 operator+(double lhs, const Vec2& rhs);
extern const Vec2 operator-(double lhs, const Vec2& rhs);
extern const Vec2 operator*(double lhs, const Vec2& rhs);
extern const Vec2 operator/(double lhs, const Vec2& rhs);

namespace std {
    template<>
    struct hash<Vec2> {
        std::size_t operator() (const Vec2& v) const {
            const int64_t *x, *y;

            x = reinterpret_cast<const int64_t*>(&v.x);
            y = reinterpret_cast<const int64_t*>(&v.y);

            return (*x << 3) ^ *y;
        }
    };

    template<>
    struct hash<Coord> {
        std::size_t operator() (const Coord& c) const {
            return (c.x << sizeof(int)/2) ^ c.y;
        }
    };
}

#endif // VEC2_H

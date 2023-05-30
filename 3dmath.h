#ifndef MATH3D_H_INCLUDED
#define MATH3D_H_INCLUDED
//#include <glm/glm.hpp>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#include <string>

//#include "btBulletDynamicsCommon.h"
#include <memory>
//#include <glm/gtx/quaternion.hpp>
#include "vec2.h"

struct Vec3 {
    double x, y, z;
    constexpr Vec3 (double nx, double ny, double nz)
        : x(nx), y(ny), z(nz) {}
    constexpr Vec3()
        : x(0), y(0), z(0) {}
    explicit constexpr Vec3(double k)
        : x(k), y(k), z(k) {}
    

	/*	GLM
	Vec3(glm::vec3 v)
        : Vec3(v.x, v.y, v.z) {}

    static const Vec3 GlmXYZ(glm::vec4 v) {
        return Vec3(v.x, v.y, v.z);
    }
    const btVector3 toBtVector3() const {
        return btVector3(x, y, z);
    }*/

    //dot product
    double dot(const Vec3& in) const {
        return x*in.x + y*in.y + z*in.z;
    }
    //cross product
    const Vec3 cross(const Vec3& in) const {
        return Vec3(y*in.z-z*in.y, -x*in.z+z*in.x, x*in.y-y*in.x);
    }
    //magnitude
    constexpr double mag() const {
        return sqrt(x*x+y*y+z*z);
    }
    constexpr Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }
    constexpr  Vec3 operator*(double in) const {
        return Vec3(x*in, y*in, z*in);
    }
    constexpr  Vec3 operator/(double in) const {
        return Vec3(x/in, y/in, z/in);
    }
    constexpr  Vec3 operator+(double in) const {
        return Vec3(x+in, y+in, z+in);
    }
    constexpr  Vec3 operator-(double in) const {
        return Vec3(x-in, y-in, z-in);
    }
    constexpr  Vec3 operator*(const Vec3& other) const {
        return Vec3(x*other.x, y*other.y, z*other.z);
    }
    constexpr  Vec3 normalize() const {
        return (*this)*(1.0/mag());
    }
    constexpr  Vec3 safeNormalize(double epsilon = 0.0001) const {
        return (*this) / (mag() + epsilon);
    }
    constexpr Vec3 operator+(const Vec3& in) const {
        return Vec3(x+in.x, y+in.y, z+in.z);
    }
    constexpr Vec3 operator-(const Vec3& in) const {
        return *this + (-in);
    }
    constexpr Vec3 abs() const {
        return Vec3(std::abs(x), std::abs(y), std::abs(z));
    }
    constexpr Vec3 piecewiseMax(const Vec3& other) const {
        return Vec3(std::max(x, other.x), std::max(y, other.y), std::max(z, other.z));
    }
    constexpr Vec3 piecewiseMin(const Vec3& other) const {
        return Vec3(std::min(x, other.x), std::min(y, other.y), std::min(z, other.z));
    }
    constexpr Vec3 piecewiseClamp(const Vec3& lo, const Vec3& hi) const {
        return this->piecewiseMax(lo).piecewiseMin(hi);
    }
    constexpr Vec3 operator/(const Vec3& in) const {
        return Vec3(x / in.x, y / in.y, z / in.x);
    }

    bool isZero() const {
        return mag() < 0.0001;
    }

    const double scalarProjOnto(const Vec3& in) const {
        return this->dot(in.normalize());
    }
    const Vec3 vectorProjOnto(const Vec3& in) const {
        return in.normalize() * scalarProjOnto(in);
    }
    const std::string toString() const {
        return std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);
    }

    double distTo(const Vec3& other) const {
        return (other - *this).mag();
    }

	/* Bullet3

    Vec3(btVector3 b)
        : x(b[0]), y(b[1]), z(b[2]) {}

    const glm::vec3 toGlmVec3() const {
        return glm::vec3(x, y, z);
    }

    const glm::vec4 toGlmVec4Dir() const {
        return glm::vec4(x, y, z, 0.);
    }
	
    const glm::vec4 toGlmVec4Pos() const {
        return glm::vec4(x, y, z, 1.);
    }
	*/

    double angleBetween(const Vec3& other) const {
        double out = acos(this->dot(other) / (this->mag() * other.mag()));

        return out == out ? out : 0.;
    }

    const Vec2 xy() const {
        return Vec2(x, y);
    }

    const Vec2 xz() const {
        return Vec2(x, z);
    }

    const Vec2 yz() const {
        return Vec2(y, z);
    }

    const Vec2 yx() const {
        return Vec2(y, x);
    }

    const Vec2 zx() const {
        return Vec2(z, x);
    }

    const Vec2 zy() const {
        return Vec2(z, y);
    }

    friend std::ostream& operator<< (std::ostream& s, const Vec3& c) {
        s << "V3("<<c.x<<", "<<c.y<<", "<<c.z<<")";
        return s;
    }

    bool valid() {
        return x == x && y == y && z == z;
    }
};


extern const Vec3 operator+(double lhs, const Vec3& rhs);
extern const Vec3 operator-(double lhs, const Vec3& rhs);
extern const Vec3 operator*(double lhs, const Vec3& rhs);
extern const Vec3 operator/(double lhs, const Vec3& rhs);

class Quaternion {
    public:
    Quaternion(const Vec3& axis, double angle);
    explicit Quaternion(const Vec3& pos);

    ///0 rotation quaternion, not <0, 0, 0, 0>
    Quaternion();

    const Vec3 rotate(const Vec3& in) const;

    //Hamilton product
    const Quaternion operator*(const Quaternion& other) const;

    ///todo: const glm::mat4 toMatrix() const;

    const Quaternion normalize() const;
    const Quaternion conjugate() const;
    const Vec3 toVec3() const;

    const std::string toString() const;

	/* glm
    const glm::mat4 toMat4() const {
        return glm::toMat4(glm::quat(w, x, y, z));
    }*/

    ///convention: ZYX
    static const Quaternion eulerAngle(Vec3 angle);

    /* bullet3

    const btQuaternion toBtQuaternion() const {
        return btQuaternion(x, y, z, w);
    }
	Quaternion(btQuaternion q)
        : x(q[0]), y(q[1]), z(q[2]), w(q[3]) {}*/

    bool valid() {
        return x == x && y == y && z == z && w == w;
    }

    double angle() const;
    const Vec3 axis() const;

    Quaternion(Vec3 startDir, Vec3 endDir);


    static Quaternion slerp(const Quaternion& a, const Quaternion& b, double t);

    private:
    double x, y, z, w;

    Quaternion(double _x, double _y, double _z, double _w);

    friend std::ostream& operator<<(std::ostream& o, const Quaternion& q) {
        return o<<"Q("<<q.x<<" "<<q.y<<" "<<q.z<<" "<<q.w<<")";
    }
};

///placement = position + orientation (vec3 + quaternion)
struct Placement {
    Vec3 pos;
    Quaternion dir;

    Placement(const Vec3& p, const Quaternion& d)
        : pos(p), dir(d) {}


    explicit Placement(const Vec3& p);

    explicit Placement(const Quaternion& q);

    Placement();

	/* glm
    glm::mat4 toMat4() const {
        return glm::translate(glm::mat4(1), pos.toGlmVec3()) * dir.toMat4();
    }
	*/

	/* bullet3

    explicit Placement(const btTransform& tf);

    const btTransform toTransform() const {
        return btTransform(dir.toBtQuaternion(), pos.toBtVector3());
    }
	*/

    ///this treated as transform, result is other transformed by this
    const Placement applyAsTransform(const Placement& other) const;

    const Placement operator-() const {
        return Placement(-pos, dir.conjugate());
    }

    bool valid() {
        return pos.valid() && dir.valid();
    }

    friend std::ostream& operator<<(std::ostream& o, const Placement& p) {
        return o<<"(P "<<p.pos<<", "<<p.dir<<")";
    }
};


#endif // MATH3D_H_INCLUDED

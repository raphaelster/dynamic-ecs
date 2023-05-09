#include "3dmath.h"

#define Q_V (axis.normalize() * sin(angle/2.0))

Quaternion::Quaternion(const Vec3& axis, double angle)
    : x(Q_V.x), y(Q_V.y), z(Q_V.z), w(cos(angle/2.0)) {}

Quaternion::Quaternion(const Vec3& pos)
    : Quaternion(pos.x, pos.y, pos.z, 0) {}

Quaternion::Quaternion(double _x, double _y, double _z, double _w)
    : x(_x), y(_y), z(_z), w(_w) {}

Quaternion::Quaternion()
    : Quaternion(Vec3(1, 0, 0), 0) {}

const Quaternion Quaternion::operator*(const Quaternion& o) const {
    return Quaternion(  w*o.x + x*o.w + y*o.z - z*o.y,      //x
                        w*o.y - x*o.z + y*o.w + z*o.x,      //y
                        w*o.z + x*o.y - y*o.x + z*o.w,      //z
                        w*o.w - x*o.x - y*o.y - z*o.z);     //w
}

const Quaternion Quaternion::normalize() const {
    double mag = std::sqrt(x*x + y*y + z*z + w*w);
    return Quaternion(x/mag, y/mag, z/mag, w/mag);
}

const Quaternion Quaternion::eulerAngle(Vec3 angle) {
    ///naive implementation; likely a way to derive the euler angle quaternion directly
    Vec3 x(1,0,0);
    Vec3 y(0,1,0);
    Vec3 z(0,0,1);

    return Quaternion(x, angle.x) * Quaternion(y, angle.y) * Quaternion(z, angle.z);
}

const std::string Quaternion::toString() const {
    return std::to_string(w) + ", <" + toVec3().toString() + ">";
}

const Quaternion Quaternion::conjugate() const {
    return Quaternion(-x, -y, -z, w);
}

const Vec3 Quaternion::toVec3() const {
    return Vec3(x, y, z);
}

const Vec3 Quaternion::rotate(const Vec3& in) const {
    return ((*this * Quaternion(in)) * this->conjugate()).toVec3();
}

const Vec3 operator+(double lhs, const Vec3& rhs) {
    return rhs + lhs;
}

const Vec3 operator-(double lhs, const Vec3& rhs) {
    return -rhs + lhs;
}

const Vec3 operator*(double lhs, const Vec3& rhs) {
    return rhs * lhs;
}

const Vec3 operator/(double lhs, const Vec3& rhs) {
    return Vec3(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
}


Placement::Placement(const Vec3& p)
    : pos(p), dir() {}


Placement::Placement(const Quaternion& q)
    : pos(), dir(q) {}

/*bullet3 compatibility
Placement::Placement(const btTransform& tf)
    : pos(tf.getOrigin()), dir(tf.getRotation()) {}
*/

Placement::Placement()
    : pos(0), dir() {}

const Placement Placement::applyAsTransform(const Placement& other) const {
    //rotate first, then translate
    Quaternion newRot = other.dir * this->dir;
    Vec3 newPos = this->pos + newRot.rotate(other.pos);

    return Placement(newPos, newRot);
}

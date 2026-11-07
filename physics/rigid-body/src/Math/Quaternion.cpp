#include "Math/Quaternion.hpp"
#include <cmath>

Quaternion::Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

void Quaternion::set(float x, float y, float z, float w) { this->x = x; this->y = y; this->z = z; this->w = w; }

float Quaternion::getx() const { return x; }
float Quaternion::gety() const { return y; }
float Quaternion::getz() const { return z; }
float Quaternion::getw() const { return w; }

Quaternion Quaternion::normalize() const {
    float magnitude = std::sqrt( x * x + y * y + z * z + w * w );
    if (magnitude == 0.0f) { return Quaternion(0.0f, 0.0f, 0.0f, 1.0f); }
    return Quaternion( x / magnitude, y / magnitude, z / magnitude, w / magnitude );
}

Quaternion Quaternion::multiplication(const Quaternion& other) const {
    return Quaternion(
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y - x * other.z + y * other.w + z * other.x,
        w * other.z + x * other.y - y * other.x + z * other.w,
        w * other.w - x * other.x - y * other.y - z * other.z
    );
}
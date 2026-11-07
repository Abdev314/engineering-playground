#include "Math/Vector3.hpp"
#include <cmath>

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

void Vector3::setx(float x) {this -> x = x;}
float Vector3::getx() const {return x;}

void Vector3::sety(float y) {this -> y = y;}
float Vector3::gety() const {return y;}

void Vector3::setz(float z) {this -> z = z;}
float Vector3::getz() const {return z;}

float Vector3::magnitude() const {return std::sqrt(x*x + y*y + z*z);}

Vector3 Vector3::normalized() const {
    float mag = magnitude();
    return mag == 0.0f ? Vector3(0.0f, 0.0f, 0.0f) : Vector3(x / mag, y /mag, z / mag);
}

Vector3 Vector3::cross(const Vector3& other) const {
    return Vector3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(
        x - other.x,
        y - other.y,
        z - other.z
    );
}

Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(
        x + other.x,
        y + other.y,
        z + other.z
    );
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}



 
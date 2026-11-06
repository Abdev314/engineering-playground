#include "physics/Vector2.hpp"
#include <cmath>

Vector2::Vector2 (float x , float y) : x(x) , y(y) {}

void Vector2::setx(float x) { this->x = x;}
float Vector2::getx() const { return x;}
void Vector2::sety(float y) { this->y = y; }
float Vector2::gety() const {return y;}
float Vector2::magnitude() const { return std::sqrt(x*x + y*y); }

Vector2 Vector2::normalized() const {
    float mag = magnitude();
    return mag == 0.0f ? Vector2(0.0f, 0.0f) : Vector2(x / mag, y / mag);
}

float Vector2::dot(const Vector2& other) const {
    return x * other.x + y * other.y;
}

Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(
        x + other.x,
        y + other.y
    );
}

Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

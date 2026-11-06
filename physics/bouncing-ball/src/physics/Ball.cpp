#include "physics/Ball.hpp"
#include "physics/Vector2.hpp"
#include <stdexcept>

Ball::Ball(
    Vector2 position,
    Vector2 velocity,
    Vector2 acceleration,
    float mass,
    float radius ) : 
    
    position(position),
    velocity(velocity),
    acceleration(acceleration),
    mass(mass),
    radius(radius) {}

void Ball::setPosition(Vector2 position) { this -> position = position; }
Vector2 Ball::getPosition() const { return position; }

void Ball::setAcceleration(Vector2 acceleration) { this -> acceleration = acceleration; }
Vector2 Ball::getAcceleration() const { return acceleration; }

void Ball::setMass(float mass) { this -> mass = mass; }
float Ball::getMass() const { return mass;}

void Ball::setRadius(float radis) {
    if (radis <= 0.0f) { throw std::invalid_argument("Radius must be positive"); }
    this -> radius = radis;
}

float Ball::getRadius() const { return radius; }

void Ball::ApplyForce(const Vector2& force) {
        if (mass <= 0.0f) { return; }
        acceleration = acceleration + Vector2(
            force.getx() / mass,
            force.gety() / mass
        );
}

void Ball::Update(float deltaTime) {
    velocity = velocity + acceleration * deltaTime;
    position = position + velocity * deltaTime;
    acceleration = Vector2(0.0f, 0.0f);  
}

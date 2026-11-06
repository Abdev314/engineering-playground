#include "physics/Ball.hpp"
#include "physics/Vector2.cpp"

Ball::Ball(
    Vector2 position,
    Vector2 velocity,
    Vector2 acceleration,
    float mass,
    float radis ) : 
    
    position(position),
    velocity(velocity),
    acceleration(acceleration),
    mass(mass),
    radis(radis) {}

void Ball::setPosition(Vector2 position) { this -> position = position; }
Vector2 Ball::getPosition() const { return position; }

void Ball::setAcceleration(Vector2 acceleration) { this -> acceleration = acceleration; }
Vector2 Ball::getAcceleration() const { return acceleration; }

void Ball::setMass(float mass) { this -> mass = mass; }
float Ball::getMass() const { return mass;}

void Ball::setRadis(float radis) { this -> radis = radis; }
float Ball::getRadis() const { return radis; }

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


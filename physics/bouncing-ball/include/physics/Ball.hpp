#ifndef BALL_H
#define BALL_H
 
#include "physics/Vector2.hpp"

class Ball {

    private:
        Vector2 position;
        Vector2 velocity;
        Vector2 acceleration;
        float mass;
        float radis;

    public:
        Ball (Vector2 position, Vector2 velocity, Vector2 acceleration, float mass, float radis);

        void setPosition(Vector2 position);
        Vector2 getPosition() const;

        void setAcceleration(Vector2 acceleration);
        Vector2 getAcceleration() const;

        void setMass(float mass);
        float getMass() const;

        void setRadis(float radis);
        float getRadis() const;

        void ApplyForce(const Vector2& force);
        void Update(float deltaTime);
};
#endif

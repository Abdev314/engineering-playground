// Hold a Ball object.
// Define gravity.
// Calculate the gravitational force.
// Call Ball::ApplyForce().
// Call Ball::Update(deltaTime).

#ifndef SIMULATION_H
#define SIMULATION_H
#include <vector>

#include "physics/Ball.hpp"
#include "physics/Vector2.hpp"

class Simulation {

    private:
        std::vector<Ball> balls;
        Vector2 gravity;

    public:
        Simulation(const std::vector<Ball>& balls, const Vector2& gravity);
        void SetGravity(const Vector2& gravity);
        Vector2 getGravity() const;

        void AddBall(const Ball& ball);
        void Update(float deltaTime);
        std::vector<Ball>& GetBalls();
};
#endif


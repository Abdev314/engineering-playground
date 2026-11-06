#include "core/Simulation.hpp"
#include <vector>

Simulation::Simulation(const std::vector<Ball>& balls, const Vector2& gravity)
    : balls(balls), gravity(gravity) {}

void Simulation::SetGravity(const Vector2& gravity) { this -> gravity = gravity; }
Vector2 Simulation::getGravity() const {return gravity; }
void Simulation::AddBall(const Ball& ball) { balls.push_back(ball); }

void Simulation::Update(float deltaTime) {
    for (Ball& ball : balls) {
        Vector2 gravityForce(
            gravity.getx() * ball.getMass(),
            gravity.gety() * ball.getMass()
        );

        ball.ApplyForce(gravityForce);
        ball.Update(deltaTime);
    }
}
const std::vector<Ball>& Simulation::GetBalls() const { return balls; }
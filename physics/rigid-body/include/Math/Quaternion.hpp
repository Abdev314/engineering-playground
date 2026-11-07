#ifndef QUATERNION_H
#define QUATERNION_H

class Quaternion {

private:
    float x, y, z, w;

public:
    Quaternion(float x, float y, float z, float w);

    void set(float x, float y, float z, float w);

    float getx() const;
    float gety() const;
    float getz() const;
    float getw() const;

    Quaternion normalize() const;
    Quaternion multiplication(const Quaternion& other) const;
};

#endif
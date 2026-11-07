#ifndef VECTOR3_H
#define VECTOR3_H

class Vector3 {

    private:
        float x, y, z;
    
    public:
        Vector3(float x, float y, float z);

        void setx(float x);
        float getx() const;

        void sety(float y);
        float gety() const;

        void setz(float z);
        float getz() const;

        float magnitude() const;
        Vector3 normalized() const;
        float dot(const Vector3& other) const;
        
        Vector3 cross(const Vector3& other) const;
        Vector3 operator+(const Vector3& other) const;
        Vector3 operator-(const Vector3& other) const;
        Vector3 operator*(float scalar) const;
};   
#endif
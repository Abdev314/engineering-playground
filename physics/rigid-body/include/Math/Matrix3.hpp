#ifndef MATRIX3_H
#define MATRIX3_H

#include "Math/Vector3.hpp"

class Matrix3 {

    private:
        float m[3][3];

    public:
        Matrix3(
            float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22
        );

        void set(int row, int column, float value);
        float get(int row, int column) const;

        Vector3 operator*(const Vector3& vector) const;
        Matrix3 operator*(const Matrix3& other) const;
};

#endif
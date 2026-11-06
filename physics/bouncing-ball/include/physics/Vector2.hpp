#ifndef VECTOR_H
#define VECTOR_H

/*Include class , defines what a vector has and what it can do*/
class Vector2 {

    private: 
        float x , y ;
    
    public:
        Vector2(float x, float y);
        
        void setx(float x);
        float getx() const;

        void sety(float y);
        float gety() const;
        
        float magnitude() const; 
        Vector2 normalized() const;
        float dot(const Vector2& other) const;
};
#endif
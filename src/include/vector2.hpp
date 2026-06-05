#pragma once

struct vector2
{
    float x;
    float y;

    vector2()
    {
        x = 0.0f;
        y = 0.0f;
    }
    template<typename x_type, typename y_type>
    vector2(x_type _x, y_type _y) : x(static_cast<float>(_x)), y(static_cast<float>(_y)) {}

    bool operator!=(const vector2& other) const
    {
        return (x != other.x) || (y != other.y);
    }

    vector2 operator+(const vector2& other) const
    {
        return vector2(x + other.x, y + other.y);
    }
    vector2 operator-(const vector2& other) const
    {
        return vector2(x - other.x, y - other.y);
    }
    vector2 operator*(const vector2& other) const
    {
        return vector2(x * other.x, y * other.y);
    }
    vector2 operator/(const vector2& other) const
    {
        return vector2(x / other.x, y / other.y);
    }

    vector2 operator*(const float& other) const
    {
        return vector2(x * other, y * other);
    }

    vector2& operator+=(const vector2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    vector2& operator+=(const float& other)
    {
        x += other;
        y += other;
        return *this;
    }
    vector2& operator*=(const vector2& other)
    {
        x *= other.x;
        y *= other.y;
        return *this;
    }
};

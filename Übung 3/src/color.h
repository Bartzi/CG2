
template<typename T>
class Color
{
public:
    Color (T rgba=T(0)) { m_c [0] = rgba; m_c [1] = rgba; m_c [2] = rgba; m_c [3] = rgba; };
    Color (T r, T g, T b, T a) { m_c [0] = r; m_c [1] = g; m_c [2] = b; m_c [3] = a; };

public:
    void setColor (T r, T g, T b, T a) { m_c [0] = r; m_c [1] = g; m_c [2] = b; m_c [3] = a; };
	bool isBlack() { return (m_c[0] == T(0) && m_c[1] == T(0) && m_c[2] == T(0)) || m_c[3] == T(0); }

    inline T const operator[] (int index) const { return m_c[index]; };
    inline T&      operator[] (int index)       { return m_c[index]; };

	inline Color<T> operator+(const Color<T>& c) const { return Color<T>(m_c [0] + c[0],  m_c [1] + c[1],  m_c [2] + c[2], m_c [3] + c[3]);  };
	inline Color<T> operator*(const Color<T>& c) const { return Color<T>(m_c [0] * c[0],  m_c [1] * c[1],  m_c [2] * c[2], m_c [3] * c[3]);  };
	inline Color<T> operator*(const float f) const { return Color<T>(m_c [0] * f,  m_c [1] * f,  m_c [2] * f, m_c [3] * f);  };

    inline T getR () const { return m_c [0]; };
    inline T getG () const { return m_c [1]; };
    inline T getB () const { return m_c [2]; };
    inline T getA () const { return m_c [3]; };

    inline void setR (T r) { m_c [0] = r; };
    inline void setG (T g) { m_c [1] = g; };
    inline void setB (T b) { m_c [2] = b; };
    inline void setA (T a) { m_c [3] = a; };

private:
    T m_c [4];
};

typedef Color<float> ColorRGBA;
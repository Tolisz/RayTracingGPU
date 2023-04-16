

namespace vec {

    template<int N, typename T>
    class vector_template;

        // ------------------------------
        //            Methods
        // ------------------------------
        
        template<int N, typename T>
        double vector_template<N, T>::length() {
            double l = 0; 
            for(int i = 0; i < N; i++) 
                l += data[i] * data[i];
            return std::sqrt(l);
        }

        // ------------------------------
        //       Binary Operatros
        // ------------------------------

    template<int N, typename T>
    vector_template<N, T> operator+(const vector_template<N, T>& v1, const vector_template<N, T>& v2) noexcept {
        vector_template<N, T> v;
        for (int i = 0; i < N; i++)
            v[i] = v1[i] + v2[i];
        return v;     
    }

    template<int N, typename T>
    vector_template<N, T> operator+(const vector_template<N, T>& v, const T& scalar) noexcept {
        vector_template<N, T> v_t;
        for (int i = 0; i < N; i++)
            v_t[i] = v[i] + scalar;
        return v_t;  
    }

    template<int N, typename T>
    vector_template<N, T> operator+(const T& scalar, const vector_template<N, T>& v) noexcept {
        vector_template<N, T> v_t; 
        for (int i = 0; i < N; i++)
            v_t[i] = scalar + v[i];
        return v_t;  
    }

    template<int N, typename T>
    vector_template<N, T> operator-(const vector_template<N, T>& v1, const vector_template<N, T>& v2) noexcept {
        vector_template<N, T> v;
        for (int i = 0; i < N; i++)
            v[i] = v1[i] - v2[i];
        return v;     
    }

    template<int N, typename T>
    vector_template<N, T> operator-(const vector_template<N, T>& v, const T& scalar) noexcept {
        vector_template<N, T> v_t;
        for (int i = 0; i < N; i++)
            v_t[i] = v[i] - scalar;
        return v_t;  
    }

    template<int N, typename T>
    vector_template<N, T> operator-(const T& scalar, const vector_template<N, T>& v) noexcept {
        vector_template<N, T> v_t;
        for (int i = 0; i < N; i++)
            v_t[i] = scalar - v[i];
        return v_t;
    }

    template<int N, typename T>
    vector_template<N, T> operator*(const vector_template<N, T>& v1, const vector_template<N, T>& v2) noexcept {
        vector_template<N, T> v;
        for (int i = 0; i < N; i++)
            v[i] = v1[i] * v2[i];
        return v;    
    }

    template<int N, typename T>
    vector_template<N, T> operator*(const vector_template<N, T>& v, const T& scalar) noexcept {
        vector_template<N, T> v_t;
        for (int i = 0; i < N; i++)
            v_t[i] = v[i] * scalar;
        return v_t; 
    }
    
    template<int N, typename T>
    vector_template<N, T> operator*(const T& scalar, const vector_template<N, T>& v) noexcept {
        vector_template<N, T> v_t;
        for (int i = 0; i < N; i++)
            v_t[i] = scalar * v[i];
        return v_t; 
    }

    template<int N, typename T>
    vector_template<N, T> operator/(const vector_template<N, T>& v1, const vector_template<N, T>& v2) noexcept {
        vector_template<N, T> v;
        for (int i = 0; i < N; i++)
            v[i] = v1[i] / v2[i];
        return v;    
    }

    template<int N, typename T>
    vector_template<N, T> operator/(const vector_template<N, T>& v, const T& scalar) noexcept {
        vector_template<N, T> v_t;
        for (int i = 0; i < N; i++)
            v_t[i] = v[i] / scalar;
        return v_t; 
    }
    
    template<int N, typename T>
    vector_template<N, T> operator/(const T& scalar, const vector_template<N, T>& v) noexcept {
        vector_template<N, T> v_t;
        for (int i = 0; i < N; i++)
            v_t[i] = scalar / v[i];
        return v_t; 
    }

    template<int N, typename T>
    bool operator==(const vector_template<N, T>& v1, const vector_template<N, T>& v2) noexcept {
        for (int i = 0; i < N; i++)
            if (v1[i] != v2[i]) 
                return false;

        return true;
    }

    template<int N, typename T>
    bool operator!=(const vector_template<N, T>& v1, const vector_template<N, T>& v2) noexcept {
        return !(v1 == v2);
    }

    // ------------------------------
    //          Functions
    // ------------------------------

    template<int N, typename T>
    vector_template<N, T> unit_vector(vector_template<N, T> const& v) {
        return v / v.length();
    }

} // namespace vec
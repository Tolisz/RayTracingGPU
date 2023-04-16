#pragma once

#include <cmath>

namespace vec {

    template<int N, typename T> 
    class vector_template
    {
    private:
        // ------------------------------
        //            Data
        // ------------------------------

        // 0 - x
        // 1 - y
        // 2 - z
        // 3 - t
        T data[N];  

    public: 
        // ------------------------------
        //         Constructors
        // ------------------------------

        vector_template(): data{0} {}
        vector_template(const vector_template& v) = default;
        vector_template(vector_template&& v) = default;

        template<typename ...T2, typename std::enable_if<sizeof...(T2) == N, int>::type = 0>
        vector_template(T2 ... args) {
            set_data<N>(args...);
        }    

        // ------------------------------
        //          Operators
        // ------------------------------
        T const& operator[](const int& i) const { 
            return data[i];
        }

        T& operator[](const int& i) {
            return data[i];
        }


        // ------------------------------
        //            Methods
        // ------------------------------
        double length();

    private: 

        template<int I, typename T2, typename ...Type>
        void set_data(T2 first_argument, Type ...args) {
            data[N - I] = first_argument;
            set_data<I - 1>(args...);
        }

        template<int I>
        void set_data() {}
    };


    // ------------------------------
    //       Binary Operatros
    // ------------------------------

    template<int N, typename T>
    vector_template<N, T> operator+(const vector_template<N, T>& v1, const vector_template<N, T>& v2) noexcept;

    template<int N, typename T>
    vector_template<N, T> operator+(const vector_template<N, T>& v, const T& scalar) noexcept; 
    
    template<int N, typename T>
    vector_template<N, T> operator+(const T& scalar, const vector_template<N, T>& v) noexcept; 
    
    template<int N, typename T>
    vector_template<N, T> operator-(const vector_template<N, T>& v1, const vector_template<N, T>& v2) noexcept;

    template<int N, typename T>
    vector_template<N, T> operator-(const vector_template<N, T>& v, const T& scalar) noexcept; 

    template<int N, typename T>
    vector_template<N, T> operator-(const T& scalar, const vector_template<N, T>& v) noexcept; 

    template<int N, typename T>
    vector_template<N, T> operator*(const vector_template<N, T>& v1, const vector_template<N, T>& v2) noexcept;

    template<int N, typename T>
    vector_template<N, T> operator*(const vector_template<N, T>& v, const T& scalar) noexcept; 
    
    template<int N, typename T>
    vector_template<N, T> operator*(const T& scalar, const vector_template<N, T>& v) noexcept;

    template<int N, typename T>
    vector_template<N, T> operator/(const vector_template<N, T>& v1, const vector_template<N, T>& v2) noexcept;

    template<int N, typename T>
    vector_template<N, T> operator/(const vector_template<N, T>& v, const T& scalar) noexcept; 
    
    template<int N, typename T>
    vector_template<N, T> operator/(const T& scalar, const vector_template<N, T>& v) noexcept;  

    template<int N, typename T>
    bool operator==(const vector_template<N, T>& v1, const vector_template<N, T>& v2) noexcept;

    template<int N, typename T>
    bool operator!=(const vector_template<N, T>& v1, const vector_template<N, T>& v2) noexcept;  


    // ------------------------------
    //          Functions
    // ------------------------------

    template<int N, typename T>
    vector_template<N, T> unit_vector(vector_template<N, T> const& v);

} // namespace vec


#include "vector_template.inl"
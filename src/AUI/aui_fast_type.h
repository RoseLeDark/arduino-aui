#pragma once

class aui_bit_type {
public:
    uint8_t bit : 1;
    /**
     * Ctor - set the bit to 0
     */
    aui_bit_type()  { bit = 0; }
    /**
     * Ctor - set the bit
     * @param b The state for this bit
     */
    aui_bit_type(uint8_t b) { bit = (b == 1 ? 1 : 0); }
    /**
     * copyble ctor
     */
    aui_bit_type(const aui_bit_type& b) { bit = b.bit; }

    aui_bit_type& operator = (const aui_bit_type& other) {
        bit = other.bit; return *this;
    }
    bool operator == (const aui_bit_type& other) {
        return bit == other.bit;
    }
    bool operator != (const aui_bit_type& other) {
        return bit != other.bit;
    }
    /**
     * Flip the bit from 0 to 1 or from 0 to 1
     */
    void flip() { bit = (bit ==1) ? 0 : 1; }
};

 template <uint8_t Bits, typename TBaseType, class TBITTYPE = aui_bit_type>
class aui_fast_type {
public:
    using self_type = aui_fast_type<Bits, TBaseType>;
    using value_type = TBaseType;
    using bit_type = TBITTYPE;

    /**
     * The new type
     */
    union {
        /** The real value **/
        value_type value;
        /** the value as bits */
        bit_type bits[Bits];
    };
    /**
     * Ctor
     * @param value_type the value for this type
     * @code
     * fast_type<int> a(3);
     * @endcode
     */
    explicit aui_fast_type(value_type v) : value(v) {}
    aui_fast_type(const aui_fast_type& c) : value(c.value) { }

    /**
     * Get the count of bits to set to 1
     * @return The count of bits to set to 1
     */
    unsigned int count() {
        unsigned int i;
        for(int j = 0; j < Bits; j++)
            i += bits[i].bit;
        return i;
    }
    /**
     * Get the count of bits to set to 0
     * @return The count of bits to set to 0
     */
    unsigned int zeros() {
        return Bits - count();
    }
    /**
     * Set the a bit on given position
     * @param [in] pos The position to set
     * @param [in] b The bit value to set
     */
    unsigned int set(size_t pos, bool p) {
        bits[pos].bit = p ? 1 : 0;
        return *this;
    }
    /**
     * Get the numbers of bits
     * @return The numbers of bits
     */
    size_t size() { return Bits; }

    /**
     * Operator to get the bit from given positon
     * @return The bit from given positon
     */
    value_type& operator [] (const size_t p) {
        return bits[p];
    }
    /**
     * Set this fast type value to a new value
     * @return this
     */
    self_type& operator = (value_type& v) {
        value = v; return *this;
    }

    self_type& operator = (self_type& other) {
        value = other.value; return *this;
    }
    /**
     * Is the other value with this equel
     * @return True when equel and false when not
     */
    bool operator == (self_type& other) {
        return value == other.value;
    }
    /**
     * Is the other value with this not equel
     * @return True when not equel and false when not
     */
    bool operator != (self_type& other) {
        return value != other.value;
    }
    bool operator <= (self_type& other) {
        return value <= other.value;
    }
    bool operator >= (self_type& other) {
        return value >= other.value;
    }
    bool operator < (self_type& other) {
        return value < other.value;
    }
    bool operator > (self_type& other) {
        return value > other.value;
    }

    self_type& operator += (self_type& other) {
        value += other.value; return *this;
    }
    self_type& operator -= (self_type& other) {
        value -= other.value; return *this;
    }
    self_type& operator *= (self_type& other) {
        value *= other.value; return *this;
    }
    self_type& operator &= (self_type& other) {
        value &= other.value; return *this;
    }
    self_type& operator |= (self_type& other) {
        value |= other.value; return *this;
    }
    self_type& operator ^= (self_type& other) {
        value ^= other.value; return *this;
    }
    self_type& operator <<= (self_type& other) {
        value <<= other.value; return *this;
    }
    self_type& operator >>= (self_type& other) {
        value >>= other.value; return *this;
    }
    self_type& operator /= (self_type& other) {
        value /= other.value; return *this;
    }
    self_type& operator += (value_type& v) {
        value += v; return *this;
    }
    self_type& operator -= (value_type& v) {
        value -= v; return *this;
    }
    self_type& operator *= (value_type& v) {
        value *= v; return *this;
    }
    self_type& operator /= (value_type& v) {
        value /= v; return *this;
    }
    self_type& operator <<= (value_type& v) {
        value <<= v; return *this;
    }
    self_type& operator >>= (value_type& v) {
        value >>= v; return *this;
    }
    self_type& operator - ( const self_type& rhs) {
        return self_type(*this) -= rhs;
    }
    self_type& operator + ( const self_type& rhs) {
        return self_type(*this) += rhs;
    }
    self_type& operator * ( const self_type& rhs) {
        return self_type(*this) *= rhs;
    }
    self_type& operator / ( const self_type& rhs) {
        return self_type(*this) /= rhs;
    }
    self_type& operator << ( const self_type& rhs) {
        return self_type(*this) <<= rhs;
    }
    self_type& operator >> ( const self_type& rhs) {
        return self_type(*this) >>= rhs;
    }
    self_type& operator | ( const self_type& rhs) {
        self_type result = self_type(*this);
        result.value |= rhs.value;
        return result;
    }
    self_type& operator ^ ( const self_type& rhs) {
        self_type result = self_type(*this);
        result.value ^= rhs.value;
        return result;
    }
    self_type& operator & ( const self_type& rhs) {
        self_type result = self_type(*this);
        result.value &= rhs.value;
        return result;
    }
    self_type& operator ~ () {
        self_type result = self_type(*this);
        result.value = ~result.value;
        return result;
    }

    self_type& operator ++ () {
        value++; return *this;
    }
    self_type& operator -- () {
        value--; return *this;
    }

};


#include <iostream>
#include <bitset>
#include <random>
#include <algorithm>
#include <string>
using namespace std;

const int BITSIZE = 1024;  // Kích thước bit cho mã hóa
using BigInt = bitset<BITSIZE>;

//-------------------------------------------------------------------------------------------------------------
// Hàm chuyển đổi từ BigInt thành chuỗi nhị phân
string binaryArrayToString(const BigInt& binary) {
    string result;
    for (int i = BITSIZE - 1; i >= 0; --i) {
        result += (binary[i] ? '1' : '0');
    }
    return result;
}

// Hàm chuyển đổi chuỗi nhị phân thành BigInt
BigInt stringToBinaryArray(const string& str) {
    BigInt result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[str.length() - 1 - i] == '1') {
            result.set(i);
        }
    }
    return result;
}
//-------------------------------------------------------------------------------------------------------------
// Toán tử << 
ostream& operator<<(ostream& os, const BigInt& v) {
    os << binaryArrayToString(v) << endl;
    return os;
}
// Hàm so sánh hai BigInt
int compare(const BigInt& a, const BigInt& b) {
    for (int i = BITSIZE - 1; i >= 0; --i) {
        if (a[i] > b[i]) return 1;   // `a` lớn hơn `b`
        if (a[i] < b[i]) return -1;  // `a` nhỏ hơn `b`
    }
    // Nếu tất cả các phần tử đều bằng nhau
    return 0;
}
//-------------------------------------------------------------------------------------------------------------
// A: Triên khai hàm lũy thừa mô - đun
// Hàm trừ hai số lớn BigInt
BigInt subtract(const BigInt& a, const BigInt& b) {
    BigInt result = a;
    bool borrow = false;

    for (int i = 0; i < BITSIZE; ++i) {
        bool bitA = result[i];
        bool bitB = b[i];
        result[i] = bitA ^ bitB ^ borrow;
        borrow = (!bitA && (bitB || borrow)) || (bitA && bitB && borrow);
    }
    return result;
}
// Hàm cộng hai số lớn BigInt
BigInt add(const BigInt& a, const BigInt& b) {
    BigInt result;
    bool carry = false;

    for (int i = 0; i < BITSIZE; ++i) {
        bool bitA = a[i];
        bool bitB = b[i];
        result[i] = bitA ^ bitB ^ carry;
        carry = (bitA && bitB) || (carry && (bitA || bitB));
    }
    return result;
}
// Hàm lấy các vị trí của các bit 1 lớn nhất trong BigInt
int getBitPositions(const BigInt& a) {
    int positions;
    for (int i = BITSIZE - 1; i >= 0; --i) {
        if (a[i]) {
            positions = i;  // Lưu vị trí bit 1
            return positions;
        }
    }
}
// Hàm chia lấy dư của BigInt
BigInt modulo(const BigInt& a, const BigInt& mod) {
    BigInt result = a;
    while (compare(result, mod) >= 0) {
        BigInt tempB = mod;
        int shift = getBitPositions(result) - getBitPositions(mod);
        tempB <<= shift;  // dịch trái để mod lớn bằng a để trừ nhanh hơn
        if (!compare(result, tempB))
            tempB >>= 1;
        result = subtract(result, tempB);
    }
    return result;
}
// Hàm nhân hai số lớn BigInt
BigInt multiply(const BigInt& a, const BigInt& b) {
    BigInt result;
    for (int i = 0; i < BITSIZE; ++i) {
        if (a[i]) {
            BigInt temp = b;
            temp <<= i;  // dịch trái theo bit của a
            result = add(result, temp);
        }
    }
    return result;
}
// Hàm dịch phải 1 bit (chia cho 2)
BigInt shiftRight(const BigInt& a) {
    return a >> 1;
}
// Hàm tính lũy thừa modular: base^exponent % mod
BigInt modular_exponentiation(BigInt base, BigInt exponent, BigInt mod) {
    BigInt result = BigInt(1);
    base = modulo(base, mod);
    cout << "base = " << base;

    while (exponent.any()) {  // Kiểm tra nếu có bit nào là 1
        if (exponent[0]) {    // Kiểm tra bit cuối
            result = multiply(result, base);
            cout << "result = " << result;
            result = modulo(result, mod);
            cout << "result = " << result;
        }
        exponent >>= 1;
        cout << "exponent = " << exponent;
        base = multiply(base, base);
        base = modulo(base, mod);
        cout << "base = " << base;
    }
    return result;
}
//-------------------------------------------------------------------------------------------------------------
// B: Triên khai hàm sinh s´ô nguyên t´ô ng˜âu nhiên
// Hàm kiểm tra hợp số của Miller-Rabin
bool isComposite(const BigInt& n, const BigInt& a, BigInt d) {
    BigInt x = modular_exponentiation(a, d, n);
    cout << "x = " << x;
    BigInt n_minus_1 = subtract(n, BigInt(1));

    if (x == BigInt(1) || x == n_minus_1) {
        return false;
    }

    while (d != n_minus_1) {
        x = multiply(x, x);
        cout << "x*x = " << x;
        x = modulo(x, n);
        cout << "x%n = " << x;
        d <<= 1;

        if (x == BigInt(1)) return true;
        if (x == n_minus_1) return false;
    }
    return true;
}
// Kiểm tra số nguyên tố bằng thuật toán Miller-Rabin
bool miller_rabin(const BigInt& prime, int k = 3) {
    if (prime == BigInt(0)) return false; // 0 không phải là số nguyên tố
    if (prime == BigInt(1)) return false; // 1 không phải là số nguyên tố
    if (prime == BigInt(2)) return true; // 2 là số nguyên tố
    if (prime == BigInt(3)) return true; // Số chẵn lớn hơn 2 không phải là số nguyên tố   

    BigInt d = subtract(prime, BigInt(1));
    while (!d[0]) d >>= 1;
    cout << "d = " << d;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dist(0, UINT32_MAX); // Hàm sinh số ngẫu nhiên để tắng tính ngầu nhiên

    for (int i = 0; i < k; i++) {
        BigInt a = add(BigInt(2), modulo(BigInt(dist(gen)), subtract(prime, BigInt(4))));
        cout << "a = " << a;
        if (isComposite(prime, a, d)) {
            return false;
        }
    }
    return true;
}
// Hàm kiểm tra số nguyên tố an toàn
bool is_safe_Prime(const BigInt& prime) {
    BigInt p_minus_1_div_2 = shiftRight(subtract(prime, BigInt(1)));
    if (miller_rabin(prime))
        if (miller_rabin(p_minus_1_div_2))
            return true;
    return false;
}
// Hàm tính log2 của BigInt `p` để làm bước nhảy
int log2(const BigInt& prime) {
    int bits = 0;
    BigInt temp = prime;

    while (compare(temp, BigInt(0))) {
        temp = temp >> 1; // Dịch phải một bit
        bits++;
    }
    return bits - 1; // Trả về log2(p)
}
// Hàm sinh số nguyên tố an toàn theo bitsize
BigInt generate_safe_prime(int bitsize) {
    BigInt prime;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
    int i = 0;
    do {
        for (int i = 0; i < bitsize; ++i) {
            prime[i] = dist(gen) % 2;
        }
        prime[bitsize - 1] = 1;  // Đảm bảo bit cao nhất là 1
        prime[0] = 1;            // Đảm bảo bit thấp nhất là 1
        cout << i++ << "--------> " << prime;

        // Tính log2(p) của `prime` và đặt bước tăng là `2 * log2(p)`
        int step_value = log2(prime);
        BigInt step = BigInt(step_value * 2);

        while (!is_safe_Prime(prime)) {
            cout << i++ << "--------> " << prime;
            prime = add(prime, step); // Tăng `prime` lên `2 * log2(p)` sau mỗi lần kiểm tra thất bại
            step_value = log2(prime);
            step = BigInt(step_value * 2);
        }
    } while (!is_safe_Prime(prime));

    return prime;
}
//-------------------------------------------------------------------------------------------------------------
// C: Triên khai hàm sinh khóa riêng ng˜âu nhiên
// Hàm sinh khóa riêng trong khoảng [2, p - 2]
BigInt generate_private_key(const BigInt& prime) {
    BigInt max_limit = subtract(prime, BigInt(2));
    BigInt private_key;

    do {
        for (int i = 0; i < BITSIZE; ++i) {
            private_key[i] = rand() % 2;
        }
    } while (compare(private_key, 1) < 0 || compare(private_key, max_limit) > 0);

    return private_key;
}
//-------------------------------------------------------------------------------------------------------------
// D: Hoàn thành logic trao đổi khóa Diffie - Hellman
// Hàm chính
int main() {
    /*int bit_size = 500;

    BigInt p = generate_safe_prime(bit_size);
    BigInt g = BigInt(2);

    BigInt a = generate_private_key(p);
    BigInt b = generate_private_key(p);

    BigInt A = modular_exponentiation(g, a, p);
    BigInt B = modular_exponentiation(g, b, p);

    BigInt alice_shared_secret = modular_exponentiation(B, a, p);
    BigInt bob_shared_secret = modular_exponentiation(A, b, p);

    cout << "Bi mat chung Alice nhan duoc: " << alice_shared_secret;
    cout << "Bi mat chung Bob nhan duoc: " << bob_shared_secret;
    cout << "Qua trinh tinh toan dung khong? " << (alice_shared_secret == bob_shared_secret) << endl;*/

    string bitstring = "1000111001111011111101101110111011001111111101011000011000010110000110000010000110111000001010111000100011011111101010000100010000110101011101011010110001100010011001111011111110101111011000010010101011110001111010101101011000100000010101100010111010001100110101101101100011101110001101110001010001111100110011110111011010100111000100100100010000110001101000100001011111000110011001111111100011001011011001110001111110000101011010010";
    BigInt base(bitstring);
    bitstring = "1000001011111101110011001110010011010101001011110100101011100001001110101011001111011010111110001000101111001011111110011000111001111100110101011100000110010011010001001101110010110001001000000110110111101111100000010110010111101010011010011011111101101111101011001001110000011000010010101111100001010010110000000011111100100101000110110111010001101001000110100010100111111";
    BigInt exponent(bitstring);
    bitstring = "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001";
    BigInt mod(bitstring);
    cout << modular_exponentiation(base, exponent, mod);
    return 0;
}
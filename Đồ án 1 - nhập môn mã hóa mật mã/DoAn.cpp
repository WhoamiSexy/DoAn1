#include <iostream>
#include <bitset>
#include <random>
#include <algorithm>
#include <string>

using namespace std;
const int BITSIZE = 1024;  // Kích thước bit cho mã hóa
using BigInt = bitset<BITSIZE>;

//-------------------------------------------------------------------------------------------------------------
// Hàm chuyển đổi từ BigInt thành chuỗi nhị phân (thay vì int)
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

// Toán tử << được hỗ trợ tự động với std::bitset, không cần định nghĩa lại
//-------------------------------------------------------------------------------------------------------------
ostream& operator<<(ostream& os, const BigInt& v) {
    os << binaryArrayToString(v) << endl;
    return os;
}
// Hàm so sánh hai BigInt
//bool operator==(const BigInt& a, const BigInt& b) {
//    return a.to_ulong() == b.to_ulong();
//}
int compare(const BigInt& a, const BigInt& b) {
    for (int i = BITSIZE - 1; i >= 0; --i) {
        if (a[i] > b[i]) return 1;   // `a` lớn hơn `b`
        if (a[i] < b[i]) return -1;  // `a` nhỏ hơn `b`
    }
    // Nếu tất cả các phần tử đều bằng nhau
    return 0;
}
// Hàm trừ hai số lớn 512 bit, a -= b
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
// Hàm cộng hai số lớn 512 bit
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
// Hàm lấy các vị trí của các bit 1 trong BigInt
int getBitPositions(const BigInt& a) {
    int positions;
    for (int i = 0; i < BITSIZE; ++i) {
        if (a[i]) {
            positions = i;  // Lưu vị trí bit 1
        }
    }
    return positions;
}
// Hàm chia lấy dư của 512-bit số lớn
BigInt modulo(const BigInt& a, const BigInt& mod) {
    BigInt result = a;
    while (compare(result, mod) >= 0) {
        cout << "Chay modul" << endl;
        BigInt tempB = mod;
        int shift = getBitPositions(a) - getBitPositions(mod) - 1;
        tempB <<= shift;  // dịch trái để mod lớn bằng a
        result = subtract(result, tempB);
    }
    return result;
}
// Hàm nhân hai số lớn
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
    cout << result;
    cout << base;
    int i = 0;
    while (exponent.any()) {  // Kiểm tra nếu có bit nào là 1
        cout << i++ << endl;
        if (exponent[0]) {    // Kiểm tra bit cuối
            result = multiply(result, base);
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

// Hàm kiểm tra hợp số bằng Miller-Rabin
bool isComposite(const BigInt& n, const BigInt& a, BigInt d) {
    BigInt x = modular_exponentiation(a, d, n);
    BigInt n_minus_1 = subtract(n, BigInt(1));

    if (x == BigInt(1) || x == n_minus_1) {
        return false;
    }

    while (d != n_minus_1) {
        x = multiply(x, x);
        x = modulo(x, n);
        d <<= 1;
        cout << x;

        if (x == BigInt(1)) return true;
        if (x == n_minus_1) return false;
    }
    return true;
}

// Kiểm tra số nguyên tố bằng thuật toán Miller-Rabin
bool miller_rabin(const BigInt& p, int k = 3) {
    if (p == BigInt(0)) return false; // 0 không phải là số nguyên tố
    if (p == BigInt(1)) return false; // 1 không phải là số nguyên tố
    if (p == BigInt(2)) return true; // 2 là số nguyên tố
    if (p == BigInt(3)) return true; // Số chẵn lớn hơn 2 không phải là số nguyên tố   

    BigInt d = subtract(p, BigInt(1));
    while (!d[0]) d >>= 1;

    for (int i = 0; i < k; i++) {
        BigInt a = add(BigInt(2), modulo(BigInt(rand()), subtract(p, BigInt(4))));

        if (isComposite(p, a, d)) {
            cout << 2;
            return false;
        }
    }
    cout << 1;
    return true;
}

// Hàm kiểm tra số nguyên tố an toàn
bool is_safe_Prime(const BigInt& p) {
    BigInt p_minus_1_div_2 = shiftRight(subtract(p, BigInt(1)));
    if (miller_rabin(p))
        if (miller_rabin(p_minus_1_div_2))
            return true;
    return false;
}
// Hàm tính log2 của BigInt `p`
int log2(const BigInt& p) {
    int bits = 0;
    BigInt temp = p;

    while (compare(temp, BigInt(0))) {
        temp = temp >> 1; // Dịch phải một bit
        bits++;
    }
    return bits - 1; // Trả về log2(p)
}
// Hàm sinh số nguyên tố an toàn
BigInt generate_safe_prime(int bitsize) {
    BigInt prime;

    do {
        for (int i = 0; i < bitsize; ++i) {
            prime[i] = rand() % 2;
        }
        prime[bitsize - 1] = 1;  // Đảm bảo bit cao nhất là 1
        prime[0] = 1;            // Đảm bảo bit thấp nhất là 1

        // Tính log2(p) của `prime` và đặt bước tăng là `2 * log2(p)`
        int step_value = log2(prime);
        BigInt step = BigInt(step_value * 2);

        // Tăng `prime` lên `2 * log2(p)` sau mỗi lần kiểm tra thất bại
        while (!is_safe_Prime(prime)) {
            prime = add(prime, step); // prime += 2 * log2(p)
            step_value = log2(prime);
            step = BigInt(step_value * 2);
        }
    } while (!is_safe_Prime(prime));

    return prime;
}

// Hàm sinh khóa riêng trong khoảng [2, p - 2]
BigInt generate_private_key(const BigInt& p) {
    BigInt max_limit = subtract(p, BigInt(2));
    BigInt private_key;

    do {
        for (int i = 0; i < BITSIZE; ++i) {
            private_key[i] = rand() % 2;
        }
    } while (compare(private_key, 1) < 0 || compare(private_key, max_limit) > 0);

    return private_key;
}

// Hàm chính
int main() {
    int bit_size = BITSIZE;

    //BigInt p = generate_safe_prime(bit_size);
    BigInt g = BigInt(2);
    //BigInt a = generate_private_key(p);
    //BigInt b = generate_private_key(p);
    //string bitString = "1000000001"; // số 513
    //BigInt a(bitString);    

    //bitString = "100111100111010011010001101101111001000111100000011111100100100001110111010111101010001001100100110011110101010100110100011111011100101001001001111100011100000001010001001000001100100111000111100111111111010000101011010101110001011101110011100110011000010100100110011101101110110101101001111101001000001010000000010001101001000111010111100010110010000100110011001101100110100010000101001111011100101001111000001010000011010";
    //BigInt p(bitString);
    //cout << p;
    //BigInt A = modular_exponentiation(g, a, p); // 2 mũ 511 chia lấy dư cho số 512 bit sẽ ra 2 mũ 511 đúng
    //cout << "A = " << A;


    string bitString = "100000";
    BigInt a(bitString);
    bitString = "10111"; 
    BigInt b(bitString);
    cout << modulo(a, b);
    
    //BigInt B = modular_exponentiation(g, b, p);

    //BigInt alice_shared_secret = modular_exponentiation(B, a, p);
    //BigInt bob_shared_secret = modular_exponentiation(A, b, p);

    //cout << "Bi mat chung Alice nhan duoc: " << alice_shared_secret;
    //cout << "Bi mat chung Bob nhan duoc: " << bob_shared_secret;
    //cout << "Qua trinh tinh toan dung khong? " << (alice_shared_secret == bob_shared_secret) << endl;
    return 0;
}
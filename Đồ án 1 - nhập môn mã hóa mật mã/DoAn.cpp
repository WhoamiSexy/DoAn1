#include <iostream>
#include <bitset>
#include <random>
#include <algorithm>
#include <string>
using namespace std;

const int BITSIZE = 1024;  // Kích thước bit cho mã hóa chung cho mỗi phần tử để tránh tràn số
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
// A: Triển khai hàm lũy thừa mô - đun
// Hàm trừ hai số lớn BigInt
BigInt subtract(const BigInt& a, const BigInt& b) {
    BigInt result;
    bool borrow = false;

    for (int i = 0; i < BITSIZE; ++i) {
        bool bitA = a[i];
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
// Hàm lấy các vị trí bit 1 lớn nhất trong BigInt
int getBitPositions(const BigInt& a) {
    int positions;
    for (int i = BITSIZE - 1; i >= 0; --i) {
        if (a[i]) {
            positions = i;  // Lưu vị trí bit 1
            return positions;
        }
    }
    return 0;
}
// Hàm chia lấy dư của BigInt
BigInt modulo(const BigInt& a, const BigInt& mod) {
    BigInt result = a;
    while (compare(result, mod) >= 0) {
        BigInt tempB = mod;
        int shift = getBitPositions(result) - getBitPositions(mod);
        tempB <<= shift;  // dịch trái để mod lớn bằng a để trừ nhanh hơn
        if (compare(result, tempB) < 0)
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

    while (exponent.any()) {  // Kiểm tra nếu có bit nào là 1
        if (exponent[0]) {    // Kiểm tra bit cuối
            result = multiply(result, base);
            result = modulo(result, mod);
        }
        exponent >>= 1;
        base = multiply(base, base);
        base = modulo(base, mod);
    }
    return result;
}
//-------------------------------------------------------------------------------------------------------------
// B: Triên khai hàm sinh s´ô nguyên t´ô ng˜âu nhiên
// Hàm kiểm tra hợp số của Miller-Rabin
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

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dist(0, UINT32_MAX); // Hàm sinh số ngẫu nhiên để tắng tính ngầu nhiên

    for (int i = 0; i < k; i++) {
        BigInt a = add(BigInt(2), modulo(BigInt(dist(gen)), subtract(prime, BigInt(4))));
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
        // Tính log2(p) của `prime` và đặt bước tăng là `2 * log2(p)`
        int step_value = log2(prime);
        BigInt step = BigInt(step_value * 2);

        while (!is_safe_Prime(prime)) {
            cout << i++ << "----->" << prime;
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
        for (int i = 0; i < getBitPositions(prime) - 1; ++i) {
            private_key[i] = rand() % 2;
        }
    } while (compare(private_key, BigInt(1)) < 0 || compare(private_key, max_limit) > 0);
    return private_key;
}
//-------------------------------------------------------------------------------------------------------------
// D: Hoàn thành logic trao đổi khóa Diffie - Hellman
// Hàm chính
int main() {
    int bit_size = 512;

    //BigInt p = generate_safe_prime(bit_size);
    BigInt p("11100101101010100100000101011011100001000001010011110100010101101011001111100100100111111000010111110000110100010010010111010110001010011001110111011101011111110000010110001000111100010100110111111111100000001110110000110101010101000010100111100110010100010100100000010001000010001011101011100001001100111101010111101001111110010001000011001100011000100011010101110001100001011100110101101110100101011001110111110100001000001010000001010101111110011011110110001001011011110111101111110011101000000010001010110011");
    BigInt g = BigInt(2);

    BigInt a = generate_private_key(p);
    BigInt b = generate_private_key(p);

    BigInt A = modular_exponentiation(g, a, p);
    BigInt B = modular_exponentiation(g, b, p);

    BigInt alice_shared_secret = modular_exponentiation(B, a, p);
    BigInt bob_shared_secret = modular_exponentiation(A, b, p);

    cout << "Bi mat chung Alice nhan duoc: " << alice_shared_secret;
    cout << "Bi mat chung Bob nhan duoc: " << bob_shared_secret;
    cout << "Qua trinh tinh toan dung khong? " << (alice_shared_secret == bob_shared_secret) << endl;
    return 0;
}
// Với p = 10100000100011010000111100010010111101111100001110011011110000100000010001100010101000101001111100101110001010111100010011000111010100000101010111000100100110011001000101101001010010101100011010111100100001011111110000101001101011001000111011110001100101110000010100000001101100011101101110101111101111100100101010110100111000111111011111011011000111011011111110110000000001011001011001001011101111011110001110110011001000100011001110010001010011000101111100100100000000111000111100111100001000111100101010101111
// thì mã Bob và Alice là 10000101010101100011100100011011001110100111110011100001000000100011101111111000100101100100111110110100100010101111001111101110000000111111111010111011100010100011001001101110101011000010101011010010100101110111000011101001011100101000011101011000100101010010100000110000001010000011011000011010010000101001011101111011010000101010101011100101010111111100101100010001110001000001101001111001010101110000001000011000110000101111111000110001011001000011111000100101011100111100010100000110101010101101111011111000
// và không đổi sau mỗi lần chạy

// Với p = 11100101101010100100000101011011100001000001010011110100010101101011001111100100100111111000010111110000110100010010010111010110001010011001110111011101011111110000010110001000111100010100110111111111100000001110110000110101010101000010100111100110010100010100100000010001000010001011101011100001001100111101010111101001111110010001000011001100011000100011010101110001100001011100110101101110100101011001110111110100001000001010000001010101111110011011110110001001011011110111101111110011101000000010001010110011
// thì mã Bob và Alice là 111111101000011111001010000011011111111100111011110111011111110111010110001111110011100111100101011010101010101100100001001010010110100101001111111110010000101100100110000011110000111111001011010011101000110110110001100010010010111111001010011110011001011001011010101001110111011101100001101001110010111011101000111001010110110111100011110110000000100010101101110010001010100001011111100001111111011111100001110000100100101001100010011001100100010111111001010101001011111101000101010010100111111111111011100011
// và không đổi sau mỗi lần chạy
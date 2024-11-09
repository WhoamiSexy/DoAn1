#include <iostream>
#include <string>
#include <vector>
#include<random>
#include <bitset>
#include <sstream>
using namespace std;
//-------------------------------------------------------------------------------------------------------------
bool operator==(const vector<uint32_t>& a, const vector<uint32_t>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}
bool operator!=(const vector<uint32_t>& a, const vector<uint32_t>& b) {
    if (a.size() != b.size()) return true;
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) return true;
    }
    return false;
}
ostream& operator<<(ostream& os, const vector<uint32_t>& v) {
    for (auto it = v.rbegin(); it != v.rend(); ++it) { // In từ phần tử cao nhất đến thấp nhất
        os << *it << " ";
    }
    return os;
}
//-------------------------------------------------------------------------------------------------------------
// A: Triên khai hàm lũy thừa mô - đun
// Hàm so sánh hai vector 512 bit
int compare(const vector<uint32_t>& a, const vector<uint32_t>& b) {
    // So sánh kích thước trước, nếu khác nhau thì số có nhiều phần tử hơn lớn hơn
    if (a.size() > b.size()) return 1;
    if (a.size() < b.size()) return -1;

    // So sánh từ phần tử cao nhất đến thấp nhất
    for (int i = a.size() - 1; i >= 0; --i) {
        if (a[i] < b[i]) return -1;
        else if (a[i] > b[i]) return 1;
    }
    return 0;
}
// Hàm trừ hai số lớn 512 bit, a -= b
void subtract(vector<uint32_t>& a, vector<uint32_t>& b) {
    if (a.size() > b.size()) {
        b.resize(a.size(), 0);
    }
    uint64_t borrow = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        uint64_t ai = static_cast<uint64_t>(a[i]);
        uint64_t bi = (i < b.size()) ? static_cast<uint64_t>(b[i]) : 0;

        // Kiểm tra nếu ai < (bi + borrow), nghĩa là phép trừ sẽ bị âm
        if (ai < bi + borrow) {
            // Thiết lập borrow và tính toán giá trị điều chỉnh cho result
            a[i] = static_cast<uint32_t>((1ULL << 32) + ai - bi - borrow);
            borrow = 1; // Cần mượn 1 cho lần tính tiếp theo
        }
        else {
            // Nếu không bị âm, thực hiện phép trừ thông thường
            a[i] = static_cast<uint32_t>(ai - bi - borrow);
            borrow = 0; // Không cần mượn
        }
    }

    // Nếu vẫn còn borrow sau khi vòng lặp kết thúc, tiếp tục trừ nó từ các phần tử còn lại
    for (size_t i = b.size(); i < a.size() && borrow != 0; ++i) {
        if (a[i] < borrow) {
            a[i] = static_cast<uint32_t>((1ULL << 32) + a[i] - borrow);
            borrow = 1;
        }
        else {
            a[i] -= borrow;
            borrow = 0;
        }
    }

    // Loại bỏ các phần tử 0 dư thừa từ cuối `a` nếu có
    while (!a.empty() && a.back() == 0) {
        a.pop_back();
    }
    while (!b.empty() && b.back() == 0) {
        b.pop_back();
    }
    for (size_t i = 0; i < a.size(); ++i) {
        long long b = a[i];
        a[i] = static_cast<uint32_t>(b);
    }
}
// Hàm chia lấy dư của 512-bit số lớn
vector<uint32_t> modulo(vector<uint32_t>& a, vector<uint32_t>& mod) {
    vector<uint32_t> remainder = a; // Bắt đầu với phần dư bằng a

    // Kiểm tra xem mod có lớn hơn a không
    if (compare(remainder, mod) < 0) {
        return remainder; // Nếu mod lớn hơn a, trả về a
    }

    // Thực hiện phép chia lấy dư
    while (compare(remainder, mod) >= 0) {
        subtract(remainder, mod);
    }
    for (size_t i = 0; i < remainder.size(); ++i) {
        long long a = remainder[i];
        remainder[i] = static_cast<uint32_t>(a);
    }
    return remainder; // Trả về phần dư
}
// Hàm nhân modulo hai số lớn
vector<uint32_t> multiply_modulo(const vector<uint32_t>& a, const vector<uint32_t>& b, vector<uint32_t>& mod) {
    vector<uint32_t> result(a.size() + b.size(), 0);

    for (size_t i = 0; i < a.size(); ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < b.size(); ++j) {
            uint64_t prod = static_cast<uint64_t>(a[i] * b[j] + result[i + j] + carry);
            result[i + j] = static_cast<uint32_t>(prod);
            carry = static_cast<uint32_t>(prod >> 32);
        }
        result[i + b.size()] = carry;
    }
    while (!result.empty() && result.back() == 0) {
        result.pop_back();
    }
    return modulo(result, mod);
}
// Dịch phải 1 bit (chia cho 2)
vector<uint32_t> shiftRight(const vector<uint32_t>& a) {
    vector<uint32_t> result = a;
    uint32_t carry = 0;

    for (int i = a.size() - 1; i >= 0; --i) {
        uint32_t newCarry = result[i] & 1;
        result[i] = (result[i] >> 1) | (carry << 31);
        carry = newCarry;
    }
    return result;
}
// Hàm tính lũy thừa modular: base^exponent % mod
vector<uint32_t> modular_exponentiation(vector<uint32_t> base, vector<uint32_t> exponent, vector<uint32_t> mod) {
    vector<uint32_t> result;
    result.push_back(1);
    base = modulo(base, mod);
    cout <<"base sau khi base%mod=" << base << endl;

    while (exponent[0] > 0) {
        // Kiểm tra nếu exponent là số lẻ
        if (exponent[0] % 2 == 1) {
            result = multiply_modulo(result, base, mod);
            cout << "result=" << result << endl;
        }
        // Chia exponent cho 2
        exponent = shiftRight(exponent);
        cout << "exponent=" << exponent << endl;

        // Bình phương base % mod
        base = multiply_modulo(base, base, mod);
        cout << "base=" << base << endl;
    }
    return result;
}
//-------------------------------------------------------------------------------------------------------------
//B: Triên khai hàm sinh s´ô nguyên t´ô ng˜âu nhiên
// Hàm nhân đôi
void doubleVector(vector<uint32_t>& p) {
    uint32_t carry = 0;

    for (auto& value : p) {
        uint64_t result = static_cast<uint64_t>(value) * 2 + carry;  // Nhân đôi và thêm carry từ lần tính trước
        value = static_cast<uint32_t>(result);  // Lấy phần 32 bit thấp
        carry = static_cast<uint32_t>(result >> 32);  // Lấy phần carry 32 bit cao cho lần tính tiếp theo
    }

    // Nếu có phần dư carry sau khi tính, thêm vào vector
    if (carry > 0) {
        p.push_back(carry);
    }
}
// Hàm kiểm tra hợp số của Miller-Rabin
bool isComposite(vector<uint32_t> n, vector<uint32_t> a, vector<uint32_t> d) {
    vector<uint32_t> x = modular_exponentiation(a, d, n);
    vector<uint32_t> k = n;
    k[0] = k[0] - 1;

    if (x[0] == 1 || x == k) {
        return false;
    }

    while( d != k) {
        x = multiply_modulo(x, x, n);
        doubleVector(d);

        if (x[0] == 1) return true;
        if (x == k)return false;
    }
    return true;
}
// Miller Rabin
bool miller_rabin(vector<uint32_t> p, int k = 10)
{
	if (p[0] <= 1 || p[0] == 4) return false;
	if (p[0] <= 3) return true; 
	
    vector<uint32_t> d(sizeof(p));
    d = p;
    d[0] -= 1;
    vector<uint32_t> d1(sizeof(p));
    d1 = p;
    d1[0] -= 4;


    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

    vector<uint32_t> d2 = { dist(gen) };

	while (d[0] % 2 == 0) {
        d = shiftRight(d);		
	}

	for (int i = 0; i < k; i++) {
        vector<uint32_t> a = modulo(d2, d1);
        a[0] = a[0] + 2;
        if (isComposite(p, a, d)) {
            return false; // Nếu p là hợp số
        }
	}
     
    return true;
}
// Hàm kiểm tra só nguyên tố 
bool is_safe_Prime(vector<uint32_t> p)
{
    vector<uint32_t> prime_2 = p;
    prime_2[0] = prime_2[0] - 1;
    prime_2 = shiftRight(prime_2);
	return miller_rabin(p) && miller_rabin(prime_2);
}
// Hàm chính 
vector<uint32_t> generate_safe_prime(int bitsize) 
{
    vector<uint32_t> prime(bitsize / 32);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

    do
	{
        for (int i = 0; i < prime.size(); i++) {
            prime[i] = dist(gen); // Sinh giá trị ngẫu nhiên 32-bit
        }
        prime[prime.size() - 1] |= (1 << (bitsize % 32)) - 1; // Đảm bảo có đúng số bit
        prime[0] |= 1; // Đảm bảo số lẻ
	} while (is_safe_Prime(prime));
	return prime;
}
//-------------------------------------------------------------------------------------------------------------
// C: Triên khai hàm sinh khóa riêng ng˜âu nhiên
// Hàm sinh khóa riêng trong khoảng [2, p - 2]
// Hàm trừ để tính p - 2
vector<uint32_t> subtract(const vector<uint32_t>& a, uint32_t value) {
    vector<uint32_t> result = a;
    uint64_t borrow = value;

    for (size_t i = 0; i < result.size(); i++) {
        uint64_t sub_result = static_cast<uint64_t>(result[i]) - borrow;
        result[i] = sub_result & 0xFFFFFFFF;
        borrow = (sub_result >> 32) & 1; // Tính borrow nếu cần
        if (borrow == 0) break;
    }

    return result;
}
// Hàm sinh khóa riêng trong khoảng [2, p - 2]
vector<uint32_t> generate_private_key(const vector<uint32_t>& p) {
    vector<uint32_t> max_limit = subtract(p, 2); // Tính p - 2
    vector<uint32_t> private_key(p.size());

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

    do {
        for (size_t i = 0; i < private_key.size(); i++) {
            private_key[i] = dist(gen); // Sinh giá trị ngẫu nhiên 32-bit
        }
    } while (compare(private_key, { 2 }) < 0 || compare(private_key, max_limit) > 0);

    return private_key;
}
//-------------------------------------------------------------------------------------------------------------
// D: Hoàn thành logic trao đôi khóa Diffie - Hellman
//base.from_decimal_string("12345678901234567890123456789018888882345678901234567898888888888888880123456789012345678901234567890123456789012345678901234567890");
//exponent.from_decimal_string("9844444765432109876543210987654321098765432109876543210987654321098765432109876543210987654321098765432109876543");
//modulus.from_decimal_string("115792089237316195423570985008687907853269984665640564039457584007913129639937");
int main() {
	// 1. Sinh s´ô nguyên t´ô lớn p và ph`ân tử sinh g
	int bit_size = 512; // Kích thước bit ví du, có thê đi`êu chỉnh
	//vector<uint32_t> p = generate_safe_prime(bit_size); // Sinh môt s´ô nguyên t´ô
    //vector<uint32_t> g = { 2 }; // Ph`ân tử sinh, sinh viên c`ân tìm hiêu và chon giá tri khác
	//// 2. Sinh khóa riêng của Alice và Bob
	//vector<uint32_t> a = generate_private_key(p); // Khóa riêng của Alice
	//vector<uint32_t> b = generate_private_key(p); // Khóa riêng của Bob
	// 3. Tính giá tri công khai của Alice và Bob
    vector<uint32_t> p = {
    0x1A2B3C4D, 0x1B2C3D4E, 0x9A0B1C2D, 0x3E4F5A6B,
    0x7C8D9E0F, 0x1B2C3D4E, 0x5F6A7B8C, 0x9D0E1F2A,
    0x3B4C5D6E, 0x7F8A9B0C, 0x1E2F3A4B, 0x5C6D7E8F,
    0x0A1B2C3D, 0x4E5F6071, 0x8A9B0C1D, 0x2E3F4A5B
    };
    vector<uint32_t> a = {
    0x1A2B3C4D, 0x1B2C3D4E, 0x9A0B1C2D, 0x3E4F5A6B,
    0x7C8D9E0F, 0x1B2C3D4E, 0x5F6A7B8C, 0x9D0E1F2A,
    };
    vector<uint32_t> g = { 2 };
    cout <<"g="<< g << endl;
    cout <<"a="<< a << endl;
    cout <<"p="<< p << endl;
    vector<uint32_t> A = modular_exponentiation(g, a, p); // Alice tính A = g^a % p
    //vector<uint32_t> B = modular_exponentiation(g, b, p); // Bob tính B = g^b % p
    // 4. Tính bí mât chung
 //   vector<uint32_t> alice_shared_secret = modular_exponentiation(B, a, p); // Alice tính s = B^a % p
 //   vector<uint32_t> bob_shared_secret = modular_exponentiation(A, b, p); // Bob tính s = A^b % p
	//// 5. Hiên thi˙k´êt quả và xác minh r`ăng bí mât chung trùng khớp
	//cout << "Bi mat chung Alice nhan duoc : " << alice_shared_secret << "\n";
	//cout << "Bi mat chung Bob nhan duoc : " << bob_shared_secret << "\n";
	//cout << "Qua trinh tinh toan dung khong ? " << (alice_shared_secret == bob_shared_secret) << "\n";
	return 0;
}
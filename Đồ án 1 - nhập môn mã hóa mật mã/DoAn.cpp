#include<iostream>
#include<vector>
#include<random>
#include<cstdint>
using namespace std;
//-------------------------------------------------------------------------------------------------------------
// Hàm chuyển đổi vector<bool> thành số nguyên
int binaryArrayToInt(const vector<bool>& binary) {
    int value = 0;
    for (int i = 0; i < binary.size(); ++i) {
        value = (value << 1) | binary[i]; // Dịch trái và thêm bit mới
    }
    return value;
}
// Hàm chuyển đổi số nguyên thành vector<bool>
vector<bool> intToBinaryArray(int a) {
    vector<bool> binary;

    // Chuyển đổi số nguyên thành nhị phân
    while (a > 0) {
        binary.push_back(a & 1); // Thêm bit thấp nhất
        a >>= 1;                 // Chia cho 2
    }

    // Đảo ngược mảng để có thứ tự từ cao đến thấp
    reverse(binary.begin(), binary.end());
    return binary;
}
// Toán tử 
bool operator==(const vector<bool>& a, const vector<bool>& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}
bool operator!=(const vector<bool>& a, const vector<bool>& b) {
    return !(a == b); // Sử dụng toán tử == để kiểm tra
}//-------------------------------------------------------------------------------------------------------------
ostream& operator<<(ostream& os, const vector<bool>& v) {
    int k = binaryArrayToInt(v);
    os << k << endl;
    return os;
}
// A: Triên khai hàm lũy thừa mô - đun
// Hàm so sánh hai vector 512 bit
int compare(const vector<bool>& a, const vector<bool>& b) {
    // So sánh kích thước trước
    if (a.size() != b.size()) {
        return (a.size() > b.size()) ? 1 : -1;
    }

    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            return (a[i] > b[i]) ? 1 : -1;
        }
    }
    return 0;
}
// Hàm trừ hai số lớn 512 bit, a -= b
vector<bool> subtract(const vector<bool>& a, const vector<bool>& b) {
    vector<bool> result(a.size(), 0);  // Kết quả có cùng kích thước với `a`
    bool borrow = 0;                   // Dấu mượn khi trừ bit

    // Thực hiện phép trừ bit theo bit từ phải sang trái
    for (int i = 0; i < a.size(); ++i) {
        bool bitA = a[i];
        bool bitB = (i < b.size()) ? b[i] : 0; // Nếu b ngắn hơn a, thì các bit còn lại coi như 0

        // Tính giá trị bit sau khi trừ, có xét dấu mượn
        result[i] = bitA ^ bitB ^ borrow;

        // Tính giá trị của borrow cho bit tiếp theo
        borrow = (!bitA && (bitB || borrow)) || (bitA && bitB && borrow);
    }

    // Loại bỏ các bit 0 dư thừa ở đầu kết quả
    while (result.size() > 1 && result.back() == 0) {
        result.pop_back();
    }

    return result;
}// Hàm chia lấy dư của 512-bit số lớn
// Hàm công hai số lớn
vector<bool> add(const vector<bool>& a, const vector<bool>& b) {
    vector<bool> result(max(a.size(), b.size()) + 1, false);
    bool carry = false;

    for (size_t i = 0; i < result.size(); i++) {
        bool bitA = (i < a.size()) ? a[i] : false;
        bool bitB = (i < b.size()) ? b[i] : false;

        result[i] = bitA ^ bitB ^ carry; // Tính bit
        carry = (bitA && bitB) || (carry && (bitA || bitB)); // Tính carry
    }
    return result;
}
vector<bool> modulo(vector<bool>& a, vector<bool>& mod) {
    // Giả sử b là không 0
    vector<bool> result = a;

    while (compare(result, mod) >= 0) {
        // Lấy b về cùng kích thước với result
        vector<bool> tempB = mod;
        while (tempB.size() < result.size()) {
            tempB.insert(tempB.begin(), false); // Thêm bit 0 vào đầu cho đến khi bằng kích thước
        }
        result = subtract(result, tempB); // Thực hiện phép trừ
    }

    return result;
}
// Hàm nhân hai số lớn
vector<bool> multiply(vector<bool>& a, vector<bool>& b) {
    vector<bool> result(a.size() + b.size(), false);

    for (size_t i = 0; i < a.size(); i++) {
        if (a[i]) {
            // Nếu bit a[i] là 1 thì cộng b vào vị trí i
            for (size_t j = 0; j < b.size(); j++) {
                if (b[j]) {
                    result[i + j] = result[i + j] ^ b[j];
                }
            }
        }
    }

    return result;
}
// Dịch phải 1 bit (chia cho 2)
vector<bool> shiftRight(const vector<bool>& a) {
    vector<bool> result(a.size());
    bool carry = 0;

    // Duyệt qua từng bit từ trái qua phải (vị trí cao đến thấp)
    for (int i = a.size() - 1; i >= 0; --i) {
        bool newCarry = a[i];

        // Chia mỗi bit cho 2 và thêm bit "carry" từ trước (dịch phải)
        result[i] = carry;
        carry = newCarry;
    }
    return result;
}
    // Hàm tính lũy thừa modular: base^exponent % mod
vector<bool> modular_exponentiation(vector<bool> base, vector<bool> exponent, vector<bool> mod) {
    vector<bool> result = { 1 }; // Kết quả khởi tạo là 1
    base = modulo(base, mod);

    while (exponent[0] > 0) {
        // Kiểm tra nếu exponent là số lẻ
        if (exponent[0] % 2 == 1) {
            result = multiply(result, base);
            result = modulo(result, mod);     
        }
        // Chia exponent cho 2
        exponent = shiftRight(exponent);
        // Bình phương base % mod
        base = multiply(base, base);
        base = modulo(base, mod);
    }

    return result;
}
//-------------------------------------------------------------------------------------------------------------
//B: Triên khai hàm sinh s´ô nguyên t´ô ng˜âu nhiên
// Kiểm tra xem n có phải là hợp số không (sử dụng cho Miller-Rabin)
bool isComposite(vector<bool> n, vector<bool> a, vector<bool> d) {
    vector<bool> x = modular_exponentiation(a, d, n);
    vector<bool> n_minus_1 = n;
    n_minus_1 = subtract(n_minus_1, { 1 });  // d = p - 1
    int check = binaryArrayToInt(x);

    if (check == 1 || x == n_minus_1) {
        return false;
    }

    while (d != n_minus_1) {
        x = multiply(x, x);
        x = modulo(x, n); // Tính x = x^2 mod n

        d.push_back(0);  // Nhân d với 2 (tức là d *= 2)
        check = binaryArrayToInt(x);

        if (check == 1) return true;    // Nếu x == 1, p là hợp số
        if (x == n_minus_1) return false;  // Nếu x == n-1, p có thể là số nguyên tố
    }
    return true;
}
// Kiểm tra số nguyên tố bằng thuật toán Miller-Rabin
bool miller_rabin(vector<bool> p, int k = 10) {
    int check = binaryArrayToInt(p);
    if (check <= 1 || check == 4) return false;
    if (check <= 3) return true;

    vector<bool> d = p;
    d = subtract(d, { 1 });  // d = p - 1

    // Chia d cho 2 liên tục cho đến khi d trở thành số lẻ
    while (d[0] == 0) {
        d = shiftRight(d);
    }

    int n_int = binaryArrayToInt(p);
    int a = 2 + rand() % (n_int - 4);
    vector<bool> array_a = intToBinaryArray(a);

    for (int i = 0; i < k; i++) {
        if (isComposite(p, array_a, d)) {
            return false;  // Nếu p là hợp số
        }
    }
    return true;
}
// Hàm kiểm tra số nguyên tố an toàn
bool is_safe_Prime(vector<bool> p) {
    vector<bool> p_minus_1_div_2 = p;
    p_minus_1_div_2 = subtract(p_minus_1_div_2, { 1 });  // p_minus_1_div_2 = p - 1
    p_minus_1_div_2 = shiftRight(p_minus_1_div_2);
    return miller_rabin(p) && miller_rabin(p_minus_1_div_2);
}
// Hàm chính 
vector<bool> generate_safe_prime(int bitsize)
{
    vector<bool> prime(bitsize);
    do {
        // Gán các bit ngẫu nhiên cho mảng `prime` với giá trị 0 hoặc 1
        for (int i = 0; i < bitsize; i++) {
            prime[i] = rand() % 2; // Gán giá trị 0 hoặc 1 cho mỗi bit
        }

        // Đảm bảo số có đúng `bitsize` bit và là số lẻ
        prime[bitsize - 1] = 1;  // Bit cao nhất đảm bảo số bit là `bitsize`
        prime[0] = 1;            // Bit thấp nhất để đảm bảo số lẻ

    } while (!is_safe_Prime(prime));

    return prime;
}
//-------------------------------------------------------------------------------------------------------------
// C: Triên khai hàm sinh khóa riêng ng˜âu nhiên
// Hàm sinh khóa riêng trong khoảng [2, p - 2]
// Hàm trừ để tính p - 2
// Hàm sinh khóa riêng trong khoảng [2, p - 2]
vector<bool> generate_private_key(const vector<bool>& p) {
    vector<bool> max_limit = subtract(p, {0, 1}); // Tính p - 2
    vector<bool> private_key(p.size());

    do {
        for (size_t i = 0; i < private_key.size(); i++) {
            private_key[i] = rand() % 2;
        }
    } while (compare(private_key, { 0, 1 }) < 0 || compare(private_key, max_limit) > 0);

    return private_key;
}
//-------------------------------------------------------------------------------------------------------------
// D: Hoàn thành logic trao đôi khóa Diffie - Hellman
int main() {
	// 1. Sinh s´ô nguyên t´ô lớn p và ph`ân tử sinh g
	int bit_size = 512; // Kích thước bit ví du, có thê đi`êu chỉnh
	vector<bool> p = generate_safe_prime(bit_size); // Sinh môt s´ô nguyên t´ô
    vector<bool> g = { 0, 1 };
	 //2. Sinh khóa riêng của Alice và Bob
	vector<bool> a = generate_private_key(p); // Khóa riêng của Alice
	vector<bool> b = generate_private_key(p); // Khóa riêng của Bob
    cout << a << endl << b;
	 //3. Tính giá tri công khai của Alice và Bob
    vector<bool> A = modular_exponentiation(g, a, p); // Alice tính A = g^a % p
    vector<bool> B = modular_exponentiation(g, b, p); // Bob tính B = g^b % p
     //4. Tính bí mât chung
    vector<bool> alice_shared_secret = modular_exponentiation(B, a, p); // Alice tính s = B^a % p
    vector<bool> bob_shared_secret = modular_exponentiation(A, b, p); // Bob tính s = A^b % p
	 //5. Hiên thi˙k´êt quả và xác minh r`ăng bí mât chung trùng khớp
	cout << "Bi mat chung Alice nhan duoc : " << alice_shared_secret << "\n";
	cout << "Bi mat chung Bob nhan duoc : " << bob_shared_secret << "\n";
	cout << "Qua trinh tinh toan dung khong ? " << (alice_shared_secret == bob_shared_secret) << "\n";
	return 0;
}
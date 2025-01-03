# Trao đổi khóa Diffie-Hellman

## Giới thiệu
Trao đổi khóa Diffie-Hellman là một giao thức mã hóa cho phép hai bên thiết lập một khóa bí mật chung thông qua một kênh truyền thông không an toàn. Khóa bí mật này sau đó có thể được sử dụng để mã hóa các thông tin liên lạc tiếp theo bằng mã hóa đối xứng.

### Đặc điểm nổi bật
- Thiết lập khóa bí mật một cách an toàn mà không cần chia sẻ khóa trước.
- Sử dụng các nguyên lý toán học về số học mô-đun và logarithm rời rạc.
- Là nền tảng của nhiều giao thức và hệ thống mã hóa an toàn.

## Cách hoạt động

1. **Thỏa thuận ban đầu:**
   - Cả hai bên đồng ý sử dụng một số nguyên tố lớn \( p \) và một cơ sở \( g \) (một căn nguyên nguyên thủy mô-đun \( p \)).

2. **Chọn khóa riêng:**
   - Mỗi bên chọn một khóa riêng (một số ngẫu nhiên lớn):
     - Bên A chọn \( a \).
     - Bên B chọn \( b \).

3. **Tính toán khóa công khai:**
   - Mỗi bên tính toán khóa công khai bằng công thức \( g^{private\_key} \mod p \):
     - Bên A tính \( A = g^a \mod p \).
     - Bên B tính \( B = g^b \mod p \).

4. **Trao đổi khóa công khai:**
   - Bên A gửi \( A \) cho Bên B.
   - Bên B gửi \( B \) cho Bên A.

5. **Tính toán khóa bí mật chung:**
   - Cả hai bên tính toán khóa bí mật chung:
     - Bên A tính \( s = B^a \mod p \).
     - Bên B tính \( s = A^b \mod p \).
   - Kết quả là giống nhau nhờ các tính chất toán học của lũy thừa mô-đun: \( s = g^{ab} \mod p \).

## Ví dụ

### Thông số
- \( p = 23 \) (số nguyên tố lớn)
- \( g = 5 \) (căn nguyên nguyên thủy mô-đun \( p \))

### Khóa riêng
- Bên A chọn \( a = 6 \).
- Bên B chọn \( b = 15 \).

### Khóa công khai
- Bên A tính \( A = g^a \mod p = 5^6 \mod 23 = 8 \).
- Bên B tính \( B = g^b \mod p = 5^{15} \mod 23 = 19 \).

### Khóa bí mật chung
- Bên A tính \( s = B^a \mod p = 19^6 \mod 23 = 2 \).
- Bên B tính \( s = A^b \mod p = 8^{15} \mod 23 = 2 \).

Khóa bí mật chung là \( s = 2 \).

## Lưu ý bảo mật
- Bảo mật của Diffie-Hellman dựa trên độ khó của bài toán logarithm rời rạc.
- Dễ bị tấn công **Man-in-the-Middle (MITM)** nếu không sử dụng xác thực.
- Các triển khai hiện đại sử dụng số nguyên tố lớn và các biến thể đường cong elliptic để tăng cường bảo mật.

## Ứng dụng
- Giao thức SSL/TLS.
- Mạng riêng ảo (VPN).
- Các ứng dụng nhắn tin mã hóa.

## Về dự án này
Repository này chứa một triển khai giao thức trao đổi khóa Diffie-Hellman. Dự án cung cấp:
- Nền tảng lý thuyết.
- Các bước tính toán minh họa với ví dụ cụ thể.
- Triển khai mẫu bằng [ngôn ngữ lập trình bạn chọn].

Hãy thoải mái đóng góp hoặc gửi phản hồi!

---

### Thông tin thêm
Dự án này nhằm cung cấp hiểu biết rõ ràng về trao đổi khóa Diffie-Hellman và tầm quan trọng của nó trong lĩnh vực mật mã hiện đại.

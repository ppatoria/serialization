#include <vector>
#include <cstring>
#include <iostream>

 auto println(const auto& content){
     std::cout << content << std::endl;
 }

struct header{
    int length;
    friend  std::ostream& operator << (std::ostream& os, const header& h);
};
std::ostream & operator << (std::ostream& os, const header& h){
    return os << "header: length: " << h.length  << "\n";
}

struct message{
    int data;
    friend  std::ostream& operator << (std::ostream& os, const message& h);
};
std::ostream & operator << (std::ostream& os, const message& m){
    return os << "message: data: " << m.data << "\n";
}

using buffer = std::vector<uint8_t>;

template<typename T>
auto serialize (const T& data, buffer& buf)
{  
  auto initial_size = buf.size();
  buf.resize(buf.size() + sizeof(T));
  std::memcpy(buf.data() + initial_size, &data, sizeof(T));
}

struct result{ header h; message m;};

auto deserialize(buffer& buf){
    header h;
    std::memcpy(&h, buf.data(), sizeof(header));
    
    message msg;
    auto msg_ptr = buf.data() + sizeof(header);
    std::memcpy(&msg, msg_ptr, sizeof(message));
    
    return result{h, msg};
}

int main(){
    header h {108};
    message m {63};
    buffer buf;
    serialize(h, buf);
    serialize(m, buf);
    auto res = deserialize(buf);
    println(res.h);
    println(res.m);

    return 0;
}

#include <vector>
#include <cstring>
#include <iostream>
#include <bit>
#include <memory>
#include <cassert>

 auto println(const auto& content){
     std::cout << content << std::endl;
 }

struct header{
    int message_length;
    char message_type;
    int validation;
    friend  std::ostream& operator << (std::ostream& os, const header& h);
    auto operator<=>(const header&) const = default;
};
std::ostream & operator << (std::ostream& os, const header& h){
    return os   << "header: \n" 
                << "--------\n"
                << "message_length  : " << h.message_length << "\n"
                << "message_type    : " << h.message_type   << "\n"
                << "validation      : " << h.validation     << "\n";
}

struct body{
    int data;
    friend  std::ostream& operator << (std::ostream& os, const body& h);
    auto operator<=>(const body&) const = default;
};
std::ostream & operator << (std::ostream& os, const body& m){
    return os   << "message: \n"
                << "---------\n"
                << "data: " << m.data << "\n";
}

using buffer = std::vector<uint8_t>;

auto resize(buffer& buf, auto size){
    auto initial_size = buf.size();
    buf.resize(buf.size() + size);
    return initial_size;
}

template<typename T>
auto serialize (const T& data, buffer& buf)
{  
  auto initial_size/*before_resize*/ = resize(buf, sizeof(T));
  std::memcpy(buf.data() + initial_size, &data, sizeof(T));
}

struct envelope{ 
    header msg_header; 
    body   msg_body;
};

auto deserialize(buffer& buf) -> envelope{
    using namespace std;
    return envelope
    {
        *bit_cast <header*> (buf.data()),
        *bit_cast <body*>   (buf.data() + sizeof(header))
    };
}

int main(){
    header  msg_header  {108, 'N', 21};
    body    msg_body    {63};

    buffer buf;

    serialize (msg_header, buf);
    serialize (msg_body,   buf);

    auto envelope = deserialize(buf);

    assert (msg_header   == envelope.msg_header );
    assert (msg_body     == envelope.msg_body   );

    println (envelope.msg_header );
    println (envelope.msg_body   );

    return 0;
}

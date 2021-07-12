#include <vector>
#include <cstring>
#include <iostream>
#include <memory>
#include <cassert>
#include <cstddef>
#include <type_traits>

 auto println(const auto& content){
     std::cout << content << std::endl;
 }

using buffer = std::vector<std::byte>;
namespace safe{
    template<typename To, typename From> 
    auto memcpy(std::vector<To>& dst, const From& src, const auto dst_start) noexcept 
    requires(   std::is_trivially_copyable_v<From>  &&
                std::is_trivially_copyable_v<To[]> )
    {
        if(dst.size() - dst_start == sizeof(From))
        {
            std::memcpy(dst.data() + dst_start, &src, sizeof(From));
        }
    }
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

auto resize(buffer& buf, auto size){
    auto initial_size = buf.size();
    buf.resize(buf.size() + size);
    return initial_size;
}

template<typename T>
auto write (const T& data, buffer& buf)
{  
  auto sizeofData = sizeof(T);
  auto buf_begin/*initial size before_resize*/ = resize(buf, sizeofData);
  safe::memcpy(buf, data, buf_begin);
}

struct envelope{ 
    header msg_header; 
    body   msg_body;
};

auto read(buffer& buf) -> envelope{
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

    write (msg_header, buf);
    write (msg_body,   buf);

    auto envelope = read(buf);

    assert (msg_header == envelope.msg_header );
    assert (msg_body   == envelope.msg_body   );

    println (envelope.msg_header );
    println (envelope.msg_body   );

    return 0;
}

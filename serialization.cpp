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

using Bytes = std::vector<std::byte>;

namespace safe{
    template<typename To, typename From> 
    auto memcpy(std::vector<To>& dst, const From& src, const auto dst_begin)  
    requires(   std::is_trivially_copyable_v<From>  &&
                std::is_trivially_copyable_v<To[]> )
    {
        if(dst.size() - dst_begin != sizeof(From))
        {
            throw std::length_error("destination size is not equal to the source size.");
        }
        std::memcpy(dst.data() + dst_begin, &src, sizeof(From));
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

std::ostream & operator << (std::ostream& os, const body& b){
    return os   << "message: \n"
                << "---------\n"
                << "data: " << b.data << "\n";
}

template<size_t size>
auto resizeBy(Bytes& buffer){
    auto initial_size_before_resize = buffer.size();
    buffer.resize(buffer.size() + size);
    return initial_size_before_resize;
}

template<typename Data>
auto write (const Data& data, Bytes& buffer)
{  
  auto buffer_begin/*initial size before resize*/ = resizeBy<sizeof(Data)>(buffer);
  safe::memcpy(buffer, data, buffer_begin);
}

struct envelope{ 
    header msg_header; 
    body   msg_body;
};

auto read(Bytes& buffer) -> envelope{
    using namespace std;
    return envelope
    {
        *bit_cast <header*> (buffer.data()),
        *bit_cast <body*>   (buffer.data() + sizeof(header))
    };
}

struct B {
    B(B const&) {}
};

int main(){
    header  msg_header  {108, 'N', 21};
    body    msg_body    {63};

    Bytes buffer;

    write (msg_header, buffer);
    write (msg_body,   buffer);

    auto envelope = read(buffer);

    assert (msg_header == envelope.msg_header );
    assert (msg_body   == envelope.msg_body   );

    println (envelope.msg_header );
    println (envelope.msg_body   );
  
    return 0;
}

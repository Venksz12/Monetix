#include "gateway.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <memory>
#include <random>
#include <functional>
#include <sstream>
namespace beast=boost::beast; namespace http=beast::http; namespace net=boost::asio; using tcp=net::ip::tcp;
namespace monetix {
static std::string request_id() { static thread_local std::mt19937_64 g{std::random_device{}()}; return std::to_string(g()); }
static std::string upstream_host(){ return std::getenv("UPSTREAM_HOST")?std::getenv("UPSTREAM_HOST"):"host.docker.internal"; }
static unsigned short upstream_port(){ return static_cast<unsigned short>(std::atoi(std::getenv("UPSTREAM_PORT")?std::getenv("UPSTREAM_PORT"):"9000")); }

class Session: public std::enable_shared_from_this<Session> {
    tcp::socket socket_; beast::flat_buffer buffer_; http::request<http::string_body> req_;
public:
    explicit Session(tcp::socket s):socket_(std::move(s)){}
    void start(){ read(); }
    void read(){ auto self=shared_from_this(); http::async_read(socket_,buffer_,req_,[self](beast::error_code ec,std::size_t){if(!ec)self->handle();});}
    void handle(){
        const auto id=request_id(); const auto it=req_.find("X-API-Key"); const auto key=(it==req_.end()?std::string{}:it->value().to_string());
        const auto identity=authenticate_api_key(key); int remaining=0;
        if(identity.empty()){ respond(http::status::unauthorized,"invalid api key",id,0); return; }
        const bool ok=rate_limit_allow(identity,100,100.0/60.0,remaining);
        if(!ok){ respond(http::status::too_many_requests,"rate limit exceeded",id,remaining); return; }
        publish_event("api.requested","{\"request_id\":\""+id+"\"}");
        // A minimal local response is available when no upstream is configured.
        const char* uh=std::getenv("UPSTREAM_HOST"); if(!uh || !*uh) { respond(http::status::ok,"Monetix gateway OK",id,remaining); return; }
        proxy(id,remaining);
    }
    void proxy(const std::string& id,int remaining){
        auto resolver=std::make_shared<tcp::resolver>(socket_.get_executor());
        auto remote=std::make_shared<tcp::socket>(socket_.get_executor());
        auto self=shared_from_this();
        resolver->async_resolve(upstream_host(),std::to_string(upstream_port()),[self,resolver,remote,id,remaining](beast::error_code ec,tcp::resolver::results_type r){
            if(ec){self->respond(http::status::bad_gateway,"upstream resolve failed",id,remaining);return;}
            net::async_connect(*remote,r,[self,remote,id,remaining](beast::error_code ec,const tcp::endpoint&){
                if(ec){self->respond(http::status::bad_gateway,"upstream connect failed",id,remaining);return;}
                auto out=self->req_; out.set(http::field::host,remote->remote_endpoint().address().to_string());
                http::async_write(*remote,out,[self,remote,id,remaining](beast::error_code ec,std::size_t){
                    if(ec){self->respond(http::status::bad_gateway,"upstream write failed",id,remaining);return;}
                    auto rb=std::make_shared<beast::flat_buffer>(); auto rr=std::make_shared<http::response<http::string_body>>();
                    http::async_read(*remote,*rb,*rr,[self,remote,rr,id,remaining](beast::error_code ec,std::size_t){
                        if(ec){self->respond(http::status::bad_gateway,"upstream read failed",id,remaining);return;}
                        rr->set("X-Monetix-Request-Id",id); rr->set("X-RateLimit-Limit","100"); rr->set("X-RateLimit-Remaining",std::to_string(remaining));
                        rr->keep_alive(false); auto sr=std::make_shared<http::response<http::string_body>>(std::move(*rr));
                        http::async_write(self->socket_,*sr,[self,sr](beast::error_code,std::size_t){beast::error_code ignored;self->socket_.shutdown(tcp::socket::shutdown_send,ignored);});
                        publish_event("api.completed","{\"request_id\":\""+id+"\",\"status\":"+std::to_string(sr->result_int())+"}");
                    });
                });
            });
        });
    }
    void respond(http::status status,const std::string& body,const std::string& id,int remaining){
        http::response<http::string_body> r{status,req_.version()}; r.set(http::field::content_type,"text/plain"); r.set("X-Monetix-Request-Id",id); r.set("X-RateLimit-Limit","100"); r.set("X-RateLimit-Remaining",std::to_string(remaining)); r.body()=body;r.prepare_payload();
        auto out=std::make_shared<http::response<http::string_body>>(std::move(r)); auto self=shared_from_this();
        http::async_write(socket_,*out,[self,out](beast::error_code,std::size_t){beast::error_code ignored;self->socket_.shutdown(tcp::socket::shutdown_send,ignored);});
        publish_event("api.completed","{\"request_id\":\""+id+"\",\"status\":"+std::to_string(static_cast<int>(status))+"}");
    }
};
HttpGateway::HttpGateway(net::io_context& io,unsigned short p):io_(io),port_(p){}
void HttpGateway::run(){
    auto acceptor=std::make_shared<tcp::acceptor>(io_,tcp::endpoint(tcp::v4(),port_));
    std::function<void()> loop=[acceptor,&loop](){acceptor->async_accept([acceptor,&loop](beast::error_code ec,tcp::socket s){if(!ec)std::make_shared<Session>(std::move(s))->start();loop();});}; loop();
}
}

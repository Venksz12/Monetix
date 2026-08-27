#include "../common_service.hpp"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <iomanip>
#include <sstream>
using namespace drogon;
static std::string hmac(const std::string& body,const std::string& secret){unsigned char mac[EVP_MAX_MD_SIZE];unsigned len=0;HMAC(EVP_sha256(),secret.data(),secret.size(),(const unsigned char*)body.data(),body.size(),mac,&len);std::ostringstream o;for(unsigned i=0;i<len;i++)o<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)mac[i];return o.str();}
int main(){
 app().registerHandler("/health",[](const HttpRequestPtr&,auto cb){cb(monetix::json(monetix::ok("wallet")));});
 app().registerHandler("/v1/wallets/{1}",[](const HttpRequestPtr&,auto cb,std::string id){Json::Value o;o["wallet_id"]=id;o["balance_minor"]=100000;o["currency"]="INR";cb(monetix::json(o));});
 app().registerHandler("/v1/payments/webhook",[](const HttpRequestPtr& r,auto cb){
   const auto body=r->body(); const auto sig=r->getHeader("X-Monetix-Signature"); const auto expected=hmac(body,std::getenv("WEBHOOK_SECRET")?std::getenv("WEBHOOK_SECRET"):"dev-webhook-secret");
   if(sig.empty()||sig!=expected){cb(monetix::json(Json::Value{{"error","invalid signature"}},k401Unauthorized));return;}
   Json::Value o;o["status"]="accepted";o["ledger"]="append-only double-entry";cb(monetix::json(o,k202Accepted));
 });
 app().addListener("0.0.0.0",monetix::port("SERVICE_PORT",8084)).run();
}

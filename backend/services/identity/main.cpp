#include "../common_service.hpp"
#include "crypto.hpp"
#include <sodium.h>
#include <openssl/hmac.h>
#include <ctime>
using namespace drogon;
namespace {
std::string env(const char* n,const char* d){return std::getenv(n)?std::getenv(n):d;}
std::string jwt(const std::string& sub,const std::string& role){
    auto b64=[](std::string s){ static const char* a="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"; std::string o; int v=0,b=-6; for(unsigned char c:s){v=(v<<8)|c;b+=8;while(b>=0){o+=a[(v>>b)&63];b-=6;}} if(b>-6)o+=a[((v<<8)>>(b+8))&63]; return o;};
    const std::string h=b64(R"({"alg":"HS256","typ":"JWT"})");
    const std::string p=b64("{"sub":""+sub+"","role":""+role+"","exp":"+std::to_string(std::time(nullptr)+3600)+"}");
    const std::string msg=h+"."+p; unsigned char mac[EVP_MAX_MD_SIZE]; unsigned int len=0; HMAC(EVP_sha256(),env("JWT_SECRET","dev-secret").data(),env("JWT_SECRET","dev-secret").size(),(const unsigned char*)msg.data(),msg.size(),mac,&len);
    std::string sig((char*)mac,len); return msg+"."+b64(sig);
}
}
int main(){ if(sodium_init()<0) return 1;
 app().registerHandler("/health",[](const HttpRequestPtr&,auto cb){cb(monetix::json(monetix::ok("identity")));});
 app().registerHandler("/v1/auth/register",[](const HttpRequestPtr& r,auto cb){
   auto b=r->getJsonObject(); if(!b||!(*b).isMember("password")){cb(monetix::json(Json::Value{{"error","password required"}},k400BadRequest));return;}
   char hash[crypto_pwhash_STRBYTES]; const auto p=(*b)["password"].asString(); if(crypto_pwhash_str_alg(hash,p.data(),p.size(),crypto_pwhash_OPSLIMIT_MODERATE,crypto_pwhash_MEMLIMIT_MODERATE,crypto_pwhash_ALG_ARGON2ID13)!=0){cb(monetix::json(Json::Value{{"error","hash failed"}},k500InternalServerError));return;}
   Json::Value out; out["password_hash"]=hash; out["message"]="persist user in PostgreSQL users table"; cb(monetix::json(out,k201Created));
 });
 app().registerHandler("/v1/auth/login",[](const HttpRequestPtr& r,auto cb){
   auto b=r->getJsonObject(); if(!b){cb(monetix::json(Json::Value{{"error","json required"}},k400BadRequest));return;}
   Json::Value out; out["access_token"]=jwt(b->get("user_id","demo").asString(),b->get("role","Consumer").asString()); out["token_type"]="Bearer"; out["expires_in"]=3600; cb(monetix::json(out));
 });
 app().addListener("0.0.0.0",monetix::port("SERVICE_PORT",8081)).run();
}

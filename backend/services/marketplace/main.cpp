#include "../common_service.hpp"
#include <unordered_map>
#include <mutex>
using namespace drogon;
static std::mutex mu;
static std::unordered_map<std::string,Json::Value> apis;
int main(){
 app().registerHandler("/health",[](const HttpRequestPtr&,auto cb){cb(monetix::json(monetix::ok("marketplace")));});
 app().registerHandler("/v1/apis",[](const HttpRequestPtr&,auto cb){
   Json::Value a(Json::arrayValue); std::lock_guard l(mu); for(auto& [k,v]:apis){auto x=v;x["id"]=k;a.append(x);} cb(monetix::json(a));
 },{Get});
 app().registerHandler("/v1/apis",[](const HttpRequestPtr& r,auto cb){
   auto b=r->getJsonObject(); if(!b){cb(monetix::json(Json::Value{{"error","json required"}},k400BadRequest));return;}
   const std::string id=b->get("slug","api-"+std::to_string(apis.size()+1)).asString(); {std::lock_guard l(mu);apis[id]=*b;} Json::Value out=*b;out["id"]=id;cb(monetix::json(out,k201Created));
 },{Post});
 app().registerHandler("/v1/apis/{1}",[](const HttpRequestPtr&,std::function<void(const HttpResponsePtr&)>&& cb,std::string id){std::lock_guard l(mu);auto it=apis.find(id);if(it==apis.end()){cb(monetix::json(Json::Value{{"error","not found"}},k404NotFound));return;}cb(monetix::json(it->second));},{Get});
 app().registerHandler("/v1/apis/{1}/versions/{2}",[](const HttpRequestPtr&,auto cb,std::string id,std::string version){Json::Value v;v["api"]=id;v["version"]=version;v["route"]="/"+version;v["sla"]["p95_ms"]=500;cb(monetix::json(v));});
 app().addListener("0.0.0.0",monetix::port("SERVICE_PORT",8082)).run();
}

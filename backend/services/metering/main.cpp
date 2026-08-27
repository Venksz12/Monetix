#include "../common_service.hpp"
#include <unordered_set>
#include <mutex>
using namespace drogon;
static std::mutex mu; static std::unordered_set<std::string> processed;
int main(){
 app().registerHandler("/health",[](const HttpRequestPtr&,auto cb){cb(monetix::json(monetix::ok("metering")));});
 app().registerHandler("/v1/events/completed",[](const HttpRequestPtr& r,auto cb){
   auto b=r->getJsonObject(); if(!b||!b->isMember("event_id")){cb(monetix::json(Json::Value{{"error","event_id required"}},k400BadRequest));return;}
   const auto id=(*b)["event_id"].asString(); std::lock_guard l(mu); if(!processed.insert(id).second){Json::Value o;o["status"]="duplicate";cb(monetix::json(o));return;}
   Json::Value o;o["status"]="metered";o["event_id"]=id;o["usage"]=b->get("usage",1);cb(monetix::json(o,k202Accepted));
 });
 app().addListener("0.0.0.0",monetix::port("SERVICE_PORT",8085)).run();
}

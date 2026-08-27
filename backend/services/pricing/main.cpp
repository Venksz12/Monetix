#include "../common_service.hpp"
#include "pricing.hpp"
using namespace drogon;
int main(){
 app().registerHandler("/health",[](const HttpRequestPtr&,auto cb){cb(monetix::json(monetix::ok("pricing")));});
 app().registerHandler("/v1/price/quote",[](const HttpRequestPtr& r,auto cb){
   auto b=r->getJsonObject(); if(!b){cb(monetix::json(Json::Value{{"error","json required"}},k400BadRequest));return;}
   auto model=b->get("model","payg").asString(); monetix::PricingModel m=model=="subscription"?monetix::PricingModel::Subscription:model=="tiered"?monetix::PricingModel::Tiered:model=="prepaid"?monetix::PricingModel::Prepaid:model=="ai_tokens"?monetix::PricingModel::AiTokens:monetix::PricingModel::Payg;
   std::vector<monetix::PriceRule> tiers; if(m==monetix::PricingModel::Tiered){tiers={{0,0,10000},{1,0,100000},{0,0,0}};}
   auto charge=monetix::calculate_charge(m,b->get("requests",1).asInt64(),b->get("unit_minor",1).asInt64(),b->get("included",0).asInt64(),tiers,b->get("input_tokens",0).asInt64(),b->get("output_tokens",0).asInt64());
   Json::Value o;o["charge_minor"]=Json::Int64(charge);o["currency"]=b->get("currency","INR");cb(monetix::json(o));
 });
 app().addListener("0.0.0.0",monetix::port("SERVICE_PORT",8083)).run();
}

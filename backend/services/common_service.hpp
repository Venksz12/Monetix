#pragma once
#include <drogon/drogon.h>
#include <cstdlib>
#include <string>
namespace monetix {
inline int port(const char* name, int fallback) { const char* p=std::getenv(name); return p?std::atoi(p):fallback; }
inline drogon::HttpResponsePtr json(const Json::Value& v, drogon::HttpStatusCode code=drogon::k200OK) {
    auto r=drogon::HttpResponse::newHttpJsonResponse(v); r->setStatusCode(code); return r;
}
inline Json::Value ok(const std::string& service) { Json::Value v; v["service"]=service; v["status"]="ok"; return v; }
}

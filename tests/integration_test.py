#!/usr/bin/env python3
import json, os, sys, urllib.request
BASE=os.getenv("MONETIX_GATEWAY","http://localhost:8080")
def get(path):
    r=urllib.request.urlopen(urllib.request.Request(BASE+path,headers={"X-API-Key":"demo_monetix_key"}),timeout=3)
    return r.status,r.read().decode()
status,body=get("/demo/hello")
assert status==200, (status,body)
print("integration smoke test passed:", body)

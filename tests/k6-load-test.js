import http from 'k6/http';
import {check,sleep} from 'k6';
export const options={vus:20,duration:'20s',thresholds:{http_req_failed:['rate<0.05'],http_req_duration:['p(95)<500']}};
export default function(){const r=http.get('http://localhost:8080/demo/hello',{headers:{'X-API-Key':'demo_monetix_key'}});check(r,{'200':x=>x.status===200});sleep(0.05);}

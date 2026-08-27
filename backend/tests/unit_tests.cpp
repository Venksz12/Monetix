#include "money.hpp"
#include "pricing.hpp"
#include "crypto.hpp"
#include <cassert>
#include <iostream>
int main(){
 using namespace monetix;
 assert((Money{100,"INR"}+Money{25,"INR"}).minor==125);
 assert(calculate_charge(PricingModel::Payg,10,7,0)==70);
 assert(calculate_charge(PricingModel::Subscription,120,5,100)==100);
 assert(sha256_hex("monetix").size()==64);
 std::cout<<"Monetix unit tests passed\n";
}

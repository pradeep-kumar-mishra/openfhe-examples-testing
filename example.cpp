#include <openfhe.h>
#include <iostream>
using namespace lbcrypto;

int main() {
    CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
    CCParams<CryptoContextCKKSRNS> params;

    params.SetMultiplicativeDepth(2);
    params.SetScalingModSize(59);
    params.SetFirstModSize(60);
    params.SetRingDim(32768);
    params.SetSecurityLevel(SecurityLevel::HEStd_128_classic);
    params.SetScalingTechnique(FIXEDAUTO);

    auto cc = GenCryptoContext(params);
    
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    cc->Enable(ADVANCEDSHE);

    lbcrypto::PrivateKey<lbcrypto::DCRTPoly> secretKey;
    KeyPair<DCRTPoly> keys = cc->KeyGen();
    secretKey = keys.secretKey;

    cc->EvalMultKeyGen(secretKey);
    cc->EvalRotateKeyGen(secretKey, {1, 2});



    std::cout << "CryptoContext generated successfully!" << std::endl;
    return 0;
}
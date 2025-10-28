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

    KeyPair<DCRTPoly> keys = cc->KeyGen();

    cc->EvalMultKeyGen(keys.secretKey);
    cc->EvalRotateKeyGen(keys.secretKey, {1, 2});

    std::cout << "CryptoContext generated successfully!" << std::endl;

    std::vector<double> inputVector = {0.1, 0.2, 0.3, 0.4, 0.5};
    Plaintext plainInputVector = cc->MakeCKKSPackedPlaintext(inputVector);
    Ciphertext<DCRTPoly> encInputVector = cc->Encrypt(keys.publicKey, plainInputVector);

    Ciphertext<DCRTPoly> encSquaredVector = cc->EvalMult(encInputVector, encInputVector);
    Plaintext plainResult;
    cc->Decrypt(keys.secretKey, encSquaredVector, &plainResult);
    plainResult->SetLength(inputVector.size());
    std::cout << "\nDecrypted squared vector: " << plainResult << std::endl;

    return 0;
}
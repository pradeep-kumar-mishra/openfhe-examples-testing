// BGV tour: add, multiply (with modulus switching), rotate on packed integers. Kept
// small (ring dim 2^12, insecure) so a -O0 Debug OpenFHE runs it quickly. Put
// breakpoints on the [BP] lines and press F11 to step into the OpenFHE implementation.
#include <openfhe.h>
#include <iostream>
using namespace lbcrypto;

int main() {
    CCParams<CryptoContextBGVRNS> params;
    params.SetPlaintextModulus(65537);       // prime, = 1 mod 2N: enables packing
    params.SetMultiplicativeDepth(2);
    params.SetSecurityLevel(HEStd_NotSet);   // debugging only: NOT secure
    params.SetRingDim(1 << 12);

    auto cc = GenCryptoContext(params);      // [BP] parameter generation
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);

    auto keys = cc->KeyGen();
    cc->EvalMultKeyGen(keys.secretKey);
    cc->EvalRotateKeyGen(keys.secretKey, {1});

    std::vector<int64_t> x = {1, 2, 3, 4, 5, 6, 7, 8};
    Plaintext pt = cc->MakePackedPlaintext(x);
    auto ct = cc->Encrypt(keys.publicKey, pt);  // [BP]

    auto ctAdd = cc->EvalAdd(ct, ct);
    auto ctSq  = cc->EvalMult(ct, ct);          // [BP] tensor + relinearize (FLEXIBLEAUTO mod-switches first)
    auto ctRot = cc->EvalRotate(ctSq, 1);       // [BP] automorphism + key switch
    std::cout << "level after mult: " << ctSq->GetLevel() << std::endl;

    Plaintext out;
    cc->Decrypt(keys.secretKey, ctAdd, &out);   // [BP]
    out->SetLength(x.size());
    std::cout << "2x:          " << out << std::endl;
    cc->Decrypt(keys.secretKey, ctSq, &out);
    out->SetLength(x.size());
    std::cout << "x^2:         " << out << std::endl;
    cc->Decrypt(keys.secretKey, ctRot, &out);
    out->SetLength(x.size());
    std::cout << "x^2 rotated: " << out << std::endl;
    return 0;
}

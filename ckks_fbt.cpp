// CKKS functional bootstrapping (experimental in OpenFHE): refresh an RLWE (coefficient-
// encoded, BFV-like) ciphertext of 8-bit integers through CKKS while applying a lookup
// table f(x) = x^2 mod 16. Parameters follow OpenFHE's functional-bootstrapping-ckks.cpp
// (8-to-4 bit LUT); ring dim 2^12, insecure. Put breakpoints on the [BP] lines and F11.
#include <openfhe.h>
#include "math/hermite.h"
#include "schemelet/rlwe-mp.h"
#include <iostream>
using namespace lbcrypto;

int main() {
    const BigInteger QBFVInit(BigInteger(1) << 60);  // fresh RLWE modulus
    const BigInteger Q(BigInteger(1) << 47);         // RLWE modulus after mod switch
    const BigInteger Bigq(BigInteger(1) << 47);      // CKKS scaling for the conversion
    const BigInteger PInput(256), POutput(16);       // 8-bit in, 4-bit out
    const uint64_t scaleTHI = 32;                    // Hermite coefficient scaling
    const size_t order = 1;
    const uint32_t numSlots = 8, ringDim = 1 << 12;
    auto f = [](int64_t x) { return (x * x) % 16; };

    // LUT as trigonometric Hermite interpolation coefficients
    auto coeffs = GetHermiteTrigCoefficients(f, PInput.ConvertToInt(), order, scaleTHI);

    std::vector<uint32_t> levelBudget = {3, 3};
    CCParams<CryptoContextCKKSRNS> params;
    params.SetSecretKeyDist(SPARSE_ENCAPSULATED);
    params.SetSecurityLevel(HEStd_NotSet);           // debugging only: NOT secure
    params.SetScalingModSize(Bigq.GetMSB() - 1);
    params.SetFirstModSize(Bigq.GetMSB() - 1);
    params.SetScalingTechnique(FLEXIBLEAUTO);
    params.SetNumLargeDigits(3);
    params.SetBatchSize(numSlots);
    params.SetRingDim(ringDim);
    uint32_t depth = FHECKKSRNS::GetFBTDepth(levelBudget, coeffs, PInput, order, SPARSE_ENCAPSULATED);
    params.SetMultiplicativeDepth(depth);

    auto cc = GenCryptoContext(params);
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    cc->Enable(ADVANCEDSHE);
    cc->Enable(FHE);
    std::cout << "ring dim " << cc->GetRingDimension() << ", depth " << depth << std::endl;

    auto keys = cc->KeyGen();
    cc->EvalFBTSetup(coeffs, numSlots, PInput, POutput, Bigq, keys.publicKey, {0, 0},  // [BP]
                     levelBudget, 0, 0, order);
    cc->EvalBootstrapKeyGen(keys.secretKey, numSlots);
    cc->EvalMultKeyGen(keys.secretKey);

    std::vector<int64_t> x = {128, 129, 0, 3, 16, 33, 64, 255};
    // Encrypt in RLWE with a large modulus, then switch down to shrink the noise
    auto ep     = SchemeletRLWEMP::GetElementParams(keys.secretKey, depth);
    auto ctRLWE = SchemeletRLWEMP::EncryptCoeff(x, QBFVInit, PInput, keys.secretKey, ep);
    SchemeletRLWEMP::ModSwitch(ctRLWE, Q, QBFVInit);

    auto ct = SchemeletRLWEMP::ConvertRLWEToCKKS(*cc, ctRLWE, keys.publicKey, Bigq,  // [BP] RLWE -> CKKS
                                                 numSlots, depth);
    auto ctLUT = cc->EvalFBT(ct, coeffs, PInput.GetMSB() - 1, ep->GetModulus(),      // [BP] bootstrap + LUT
                             scaleTHI, 0, order);
    auto polys = SchemeletRLWEMP::ConvertCKKSToRLWE(ctLUT, Q);                       // [BP] CKKS -> RLWE

    // decryption returns centered values in [-8, 8); map them to [0, 16)
    auto y = SchemeletRLWEMP::DecryptCoeff(polys, Q, POutput, keys.secretKey, ep, numSlots, numSlots);
    std::vector<int64_t> got, expected;
    for (size_t i = 0; i < x.size(); ++i) {
        got.push_back((y[i] % 16 + 16) % 16);
        expected.push_back(f(x[i]));
    }
    std::cout << "x:          " << x << std::endl;
    std::cout << "FBT f(x):   " << got << std::endl;
    std::cout << "plain f(x): " << expected << std::endl;
    return 0;
}

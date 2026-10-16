// CKKS tour: bootstrap, multiply, rotate. Kept small (ring dim 2^12, insecure) so a
// -O0 Debug OpenFHE runs it quickly. Put breakpoints on the [BP] lines and press F11
// to step into the OpenFHE implementation.
#include <openfhe.h>
#include <iostream>
using namespace lbcrypto;

int main() {
    CCParams<CryptoContextCKKSRNS> params;
    params.SetSecretKeyDist(UNIFORM_TERNARY);
    params.SetSecurityLevel(HEStd_NotSet);   // debugging only: NOT secure
    params.SetRingDim(1 << 12);
    params.SetScalingTechnique(FLEXIBLEAUTO);
    params.SetScalingModSize(59);
    params.SetFirstModSize(60);

    std::vector<uint32_t> levelBudget = {4, 4};  // CoeffsToSlots / SlotsToCoeffs levels
    uint32_t levelsAfterBootstrap = 10;
    uint32_t depth = levelsAfterBootstrap +
                     FHECKKSRNS::GetBootstrapDepth(levelBudget, UNIFORM_TERNARY);
    params.SetMultiplicativeDepth(depth);

    auto cc = GenCryptoContext(params);
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    cc->Enable(ADVANCEDSHE);
    cc->Enable(FHE);

    uint32_t numSlots = cc->GetRingDimension() / 2;
    std::cout << "ring dim " << cc->GetRingDimension() << ", depth " << depth << std::endl;

    cc->EvalBootstrapSetup(levelBudget);                    // [BP] precompute linear transforms
    auto keys = cc->KeyGen();
    cc->EvalMultKeyGen(keys.secretKey);
    cc->EvalRotateKeyGen(keys.secretKey, {1});
    cc->EvalBootstrapKeyGen(keys.secretKey, numSlots);      // [BP] rotation + conjugation keys

    std::vector<double> x = {0.25, 0.5, 0.75, 1.0, 2.0, 3.0, 4.0, 5.0};
    // Encrypt at the last level so the ciphertext is "exhausted" before bootstrapping
    Plaintext pt = cc->MakeCKKSPackedPlaintext(x, 1, depth - 1);
    auto ct = cc->Encrypt(keys.publicKey, pt);
    std::cout << "levels remaining before: " << depth - ct->GetLevel() << std::endl;

    auto ctBoot = cc->EvalBootstrap(ct);                    // [BP] ModRaise->CtS->EvalMod->StC
    std::cout << "levels remaining after:  " << depth - ctBoot->GetLevel() << std::endl;

    auto ctSq  = cc->EvalMult(ctBoot, ctBoot);              // [BP] tensor + relinearize + rescale
    auto ctRot = cc->EvalRotate(ctSq, 1);                   // [BP] automorphism + key switch

    Plaintext out;
    cc->Decrypt(keys.secretKey, ctSq, &out);                // [BP]
    out->SetLength(x.size());
    std::cout << "x^2:         " << out << std::endl;
    cc->Decrypt(keys.secretKey, ctRot, &out);
    out->SetLength(x.size());
    std::cout << "x^2 rotated: " << out << std::endl;
    return 0;
}

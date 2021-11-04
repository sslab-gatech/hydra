// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string>
#include <cassert>

#include "MutationStage.hpp"

/*
 * AFL preserves the seed length other than havoc
 * arg mutation/function mutations are considered length-preserving
 * Appending/removing are length-modifying: let's do it only for havoc now.
 *
 * How to pick the argument when function mutation..?
 * -> Do randomly if havoc
 *
 */


//ArgMutator *argMutator;

ArgStage* argStage;
HavocStage* havocStage;
extern "C" {

uint32_t mutate_args_init(uint8_t* testcase, uint32_t len) {
    assert(argStage == nullptr);
    argStage = new ArgStage(testcase, len);
    return argStage->getStageMax();
}

uint32_t mutate_args(uint8_t* buf, uint32_t buf_size) {
    // AFL uses u32 for len
/* Takes out_buf (seed) from AFL engine as an input and mutate in-place
 * Returns new len
 * */
  return argStage->mutate(buf, buf_size);
}

void mutate_args_fini() {
    delete argStage;
    argStage = nullptr;
}



void mutate_havoc_init(uint8_t* testcase, uint32_t len, uint32_t stage_max) {
    assert(havocStage == nullptr);
    havocStage = new HavocStage(testcase, len);
    havocStage->setStageMax(stage_max);
}

uint32_t mutate_havoc(uint8_t* testcase, uint32_t len) {
    // AFL uses u32 for len
/* Takes out_buf (seed) from AFL engine as an input and mutate in-place
 * Returns new len
 * */
  return havocStage->mutate(testcase, len);
}

void mutate_havoc_fini() {
    delete havocStage;
    havocStage = nullptr;
}


}



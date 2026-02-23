#include "S_Init.hpp"

#include "../engine/AudioMgr.hpp"
#include "../engine/Global.hpp"

S_Init::S_Init() {}
void S_Init::enter() { G::audio->play("1kstar"); }
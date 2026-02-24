#include "S_Init.hpp"

#include "../engine/AudioMgr.hpp"
#include "../engine/Global.hpp"
#include "../type/Image.hpp"
#include "../type/Animation.hpp"
#include "../type/Text.hpp"

S_Init::S_Init() {
    objects["img1"] = new Image("bg_static");
    objects["img2"] = new Animation("bg_anim", 100);
    objects["img2"]->y = 170;

    objects["text1"] = new Text("main", "${main.settings}, 你好, ${myname}");
    objects["text1"]->layer = 1;
}
void S_Init::enter() { G::audio->play("1kstar"); }
#include "sandbox/sandbox.h"

#include "vertex/image/img/image.h"
#include "vertex/image/img/transform.h"
#include "vertex/image/img/color.h"
#include "vertex/image/img/mipmap.h"
#include "vertex/image/img/blit.h"

int main()
{
    using namespace vx;

    img::error_code err{};
    img::image src("../../assets/seth_frog_resized.png", err);
    img::image dst("../../assets/sunflower.png", err);

    math::blend_func blend;
    //blend.src_color_blend = blend_mode::SRC;
    //blend.dst_color_blend = blend_mode::ZERO;
    //blend.src_alpha_blend = blend_mode::ONE;
    //blend.dst_alpha_blend = blend_mode::ONE;
    //
    //blend.color_op = blend_operator::SUBTRACT;
    //blend.alpha_op = blend_operator::SUBTRACT;

    if (err == img::error_code::NONE)
    {
        img::blit(src, src.get_rect(), dst, math::vec2i(0), blend);
        //dst = img::resize(dst, math::vec2i(100));

        err = img::write_png("../../assets/blend_test.png", dst.get_info(), dst.data());
        std::cout << "image saved with error : " << (int)err << std::endl;
    }
    else
    {
        std::cout << (int)err << std::endl;
    }

    return 0;
}
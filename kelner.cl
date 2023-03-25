__kernel void ray_tracer(write_only image2d_t image)
{
    int i = get_global_id(0);   // height
    int j = get_global_id(1);   // width

    if (i == 224 && j == 399)
    {
        printf("Hello\n");
    }
    
    uint4 PixelColor = (uint4)(0, 0, 255, 255);
    int2 PixelPos = (int2)(j, i);
    
    write_imageui(image, PixelPos, PixelColor);
}

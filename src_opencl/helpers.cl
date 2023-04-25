#ifndef CL_HELPERS
#define CL_HELPERS


float3 reflect(float3 v, float3 n)
{
    return v - 2 * dot(v, n)* n;
}

float3 refract(float3 uv, float3 n, float etai_over_etat)
{
    float cos_theta = fmin(dot(-uv, n), 1.0f);
    float3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    float3 r_out_parallel = -sqrt(fabs(1.0f - dot(r_out_perp, r_out_perp))) * n;
    
    return r_out_parallel + r_out_perp;
}

float reflectance(float cosine, float ref_idx)
{
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0)*pow((1 - cosine), 5);
}

bool near_zero(float3 v)
{
    const float s = 1e-8;
    return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
}


#endif
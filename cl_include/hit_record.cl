typedef struct _Hit_Record
{
    float3 p;
    float3 normal;
    
    float t;
    bool front_face;

    int material_type;
    int sphere_id;
}
Hit_Record;
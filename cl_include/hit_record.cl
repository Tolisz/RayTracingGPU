typedef struct _Hit_Record
{
    float3 p;
    float3 normal;
    
    float t;
    bool front_face;
}
Hit_Record;
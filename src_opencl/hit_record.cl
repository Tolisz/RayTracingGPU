#ifndef CL_HIT_RECORD
#define CL_HIT_RECORD

typedef struct _Hit_Record
{
    float3 p;
    float3 normal;
    
    float t;
    bool front_face;

    int mat_id;
    int mat_num;
}
Hit_Record;

void hit_record_set_face_normal(Hit_Record* rec, Ray* r, float3 outward_normal)
{
    rec->front_face = dot(r->direction, outward_normal) < 0;
    rec->normal = rec->front_face ? outward_normal :-outward_normal;
}

#endif
#ifndef _AIMBONE_
#define _AIMBONE_

static BYTE SkeletonHitboxMatrix[12][2] =
{
	{0,1},{1,2},{2,3},{2,4},{4,5},{5,6},{6,7},{1,5},{0,6},{3,7},{7,4},{0,3},//Box
};

void SetRemapColors(struct cl_entity_s* ent);
void ClearIndex(struct ref_params_s* pparams);

#endif
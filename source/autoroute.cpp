#include "client.h"

AutoRoute autoroute;

float GetPointDistance(float* pos)
{
	double a = pos[0] - g_Local.vEye[0];
	double b = pos[1] - g_Local.vEye[1];
	double c = pos[2] - g_Local.vEye[2];
	return (float)sqrt(a * a + b * b + c * c);
}

void CalcVec_MeToTarget(float* view)
{
	float EntViewOrg[3];
	VectorCopy(view, EntViewOrg);
	view[0] = EntViewOrg[0] - g_Local.vEye[0];
	view[1] = EntViewOrg[1] - g_Local.vEye[1];
	view[2] = EntViewOrg[2] - g_Local.vEye[2];
}

void GetPointAngles(float* point, float* angles)
{
	float vec_to_target[3];
	VectorCopy(point, vec_to_target);
	CalcVec_MeToTarget(vec_to_target);
	VectorAngles(vec_to_target, angles);
	angles[0] *= -1;
	if (angles[0] > 180) angles[0] -= 360;
	if (angles[1] > 180) angles[1] -= 360;
}

void ChangeViewAngle(float* point, usercmd_s* usercmd)
{
	float aim_viewangles[3];
	float vec_to_target[3];
	VectorCopy(point, vec_to_target);
	CalcVec_MeToTarget(vec_to_target);
	VectorAngles(vec_to_target, aim_viewangles);
	aim_viewangles[0] *= -1;
	if (aim_viewangles[0] > 180) aim_viewangles[0] -= 360;
	if (aim_viewangles[1] > 180) aim_viewangles[1] -= 360;
	VectorCopy(aim_viewangles, usercmd->viewangles);
	g_Engine.SetViewAngles(aim_viewangles);
}

bool GetPointVisible(float* point)
{
	strace_t tr;
	tr.finished = false;
	TraceThickness(g_Local.vEye, point, 0, &tr);
	return tr.finished;
}

float CalcTwoAngle(float a, float b)
{
	float c = abs(a - b);
	if (c > 180)
		return abs(c - 360);
	else
		return c;
}

float CalcRouteAngle(float* point, float* point1)
{
	Vector ViewAngles;
	g_Engine.GetViewAngles(ViewAngles);
	float addvec[2];
	float dodgeDir[3];
	dodgeDir[0] = dodgeDir[1] = dodgeDir[2] = 0;
	addvec[0] = point[0] - point1[0];
	addvec[1] = point[1] - point1[1];
	float dist = (float)sqrt(addvec[0] * addvec[0] + addvec[1] * addvec[1]);
	addvec[0] /= dist;
	addvec[1] /= dist;
	dodgeDir[0] -= addvec[0];
	dodgeDir[1] -= addvec[1];
	dodgeDir[2] = 0;
	float dodgeAngles[3];
	VectorAngles(dodgeDir, dodgeAngles);
	return  CalcTwoAngle(dodgeAngles[1], ViewAngles[1]);
}

void AutoRoute::CreateMove(usercmd_s* usercmd)
{
	if (Record)
	{
		if (iRouteLine > -1)
		{
			if (iRoutePoint == routepointcount) { Record = false; return; };
			if (iRoutePoint == -1)
			{
				AddPoint();
			}
			else
			{
				float dist = GetPointDistance(route_line[iRouteLine].point[iRoutePoint].origin);
				if (dist >= 100.f)
				{
					AddPoint();
				}
			}
		}
		return;
	}

	if (bContinueAutoRoute)
	{
		if (iRouteLine > -1 && iRoutePoint > -1 && abs(route_line[iRouteLine].point[iRoutePoint].origin[2] - g_Local.vEye[2]) < 25 && GetPointVisible(route_line[iRouteLine].point[iRoutePoint].origin))
		{
			route_mode = ROUTE_START;
		}
		else
		{
			FindAnyRoute(10000, false);
		}
		bContinueAutoRoute = false;
	}

	if (cvar.route_auto && g_Local.bAlive)
	{
		if (iTargetRage && g_Local.weapon.m_iClip)
		{
			if (cvar.route_mode || IsCurWeaponKnife())
			{
				iRouteLine = -1;
				iRoutePoint = -1;
				route_mode = ROUTE_NONE;
				return;
			}
		}
		switch (route_mode)
		{
		case ROUTE_NONE:
		{
			FindFowardRoute(-1);
			if (iRoutePoint > -1 && iRouteLine > -1)
			{
				GotoRoutePoint(route_line[iRouteLine].point[iRoutePoint].origin, usercmd);
				route_mode = ROUTE_FROM;
			}
			break;
		}
		case ROUTE_START:
		{
			GotoRoutePoint(route_line[iRouteLine].point[iRoutePoint].origin, usercmd);
			route_mode = ROUTE_FROM;
		}
		break;
		case ROUTE_FROM:
		{
			if (((route_direction == ROUTE_ASC) && (iRoutePoint == route_line[iRouteLine].count - 1)) || ((route_direction == ROUTE_DESC) && (iRoutePoint == 0)))
			{
				FindFowardRoute(iRouteLine);
				if (iRoutePoint > -1 && iRouteLine > -1)
				{
					GotoRoutePoint(route_line[iRouteLine].point[iRoutePoint].origin, usercmd);
					route_mode = ROUTE_FROM;
				}
				else
					route_mode = ROUTE_NONE;
				return;
			}

			if (RouteMoveEnough(route_line[iRouteLine].point[iRoutePoint].origin, usercmd))
			{
				int i = iRoutePoint;
				switch (route_direction)
				{
				case ROUTE_ASC:
				{
					i++;
					if (g_Local.weapon.m_iClip)
					{
						if (i == route_line[iRouteLine].count - 1 && !iTargetRage)
						{
							ChangeViewAngle(route_line[iRouteLine].point[i].origin, usercmd);
						}
					}
					else
					{
						if (i == route_line[iRouteLine].count - 1)
						{
							ChangeViewAngle(route_line[iRouteLine].point[i].origin, usercmd);
						}
					}
				}
				break;
				case ROUTE_DESC:
				{
					i--;
					if (g_Local.weapon.m_iClip)
					{
						if (i == 0 && !iTargetRage)
						{
							ChangeViewAngle(route_line[iRouteLine].point[i].origin, usercmd);
						}
					}
					else
					{
						if (i == 0)
						{
							ChangeViewAngle(route_line[iRouteLine].point[i].origin, usercmd);
						}
					}
				}
				break;
				}
				iRoutePoint = i;
				GotoRoutePoint(route_line[iRouteLine].point[iRoutePoint].origin, usercmd);
			}
			return;
		}
		break;
		case ROUTE_END:
		{
			iRouteLine = -1;
			iRoutePoint = -1;
			route_mode = ROUTE_NONE;
		}
		break;
		}
	}
	else
	{
		iRouteLine = -1;
		iRoutePoint = -1;
		route_mode = ROUTE_NONE;
	}
}

void AutoRoute::GotoRoutePoint(float* point, usercmd_s* usercmd)
{
	Vector ViewAngles;
	g_Engine.GetViewAngles(ViewAngles);
	float addvec[2];
	float dodgeDir[3];
	dodgeDir[0] = dodgeDir[1] = dodgeDir[2] = 0;
	addvec[0] = g_Local.vEye[0] - point[0];
	addvec[1] = g_Local.vEye[1] - point[1];
	float dist = GetPointDistance(point);
	addvec[0] /= dist;
	addvec[1] /= dist;
	dodgeDir[0] -= addvec[0];
	dodgeDir[1] -= addvec[1];
	dodgeDir[2] = 0;
	float dodgeAngles[3];
	VectorAngles(dodgeDir, dodgeAngles);
	float angle = dodgeAngles[1] - ViewAngles[1];
	while (angle < 0) { angle += 360; }
	while (angle > 360) { angle -= 360; }
	me_forwardmove = (float)cos(angle * (3.1415926 / 180.0)) * dist;
	me_sidemove = (float)-sin(angle * (3.1415926 / 180.0)) * dist;
	usercmd->forwardmove += me_forwardmove;
	usercmd->sidemove += me_sidemove;
}

bool AutoRoute::RouteMoveEnough(float* point, usercmd_s* usercmd)
{
	Vector ViewAngles;
	g_Engine.GetViewAngles(ViewAngles);
	float addvec[2];
	float dodgeDir[3];
	dodgeDir[0] = dodgeDir[1] = dodgeDir[2] = 0;
	addvec[0] = g_Local.vEye[0] - point[0];
	addvec[1] = g_Local.vEye[1] - point[1];
	float dist = GetPointDistance(point);
	addvec[0] /= dist;
	addvec[1] /= dist;
	dodgeDir[0] -= addvec[0];
	dodgeDir[1] -= addvec[1];
	dodgeDir[2] = 0;
	float dodgeAngles[3];
	VectorAngles(dodgeDir, dodgeAngles);
	float angle = dodgeAngles[1] - ViewAngles[1];
	while (angle < 0) { angle += 360; }
	while (angle > 360) { angle -= 360; }
	me_forwardmove = (float)cos(angle * (3.1415926 / 180.0)) * dist;
	me_sidemove = (float)-sin(angle * (3.1415926 / 180.0)) * dist;
	usercmd->forwardmove = me_forwardmove * 2.5f;
	usercmd->sidemove = me_sidemove * 2.5f;
	if (usercmd->forwardmove > 0 - 230 && usercmd->forwardmove < 230 && usercmd->sidemove>0 - 230 && usercmd->sidemove < 230)
		return true;
	return false;
}

void AutoRoute::FindFowardRoute(int lastroute)
{
	float angles[3];
	float dist1;
	float dist;
	float minangles = 50;
	float minrouteangles = 30;
	float pointangles;
	float routeangle;
	iRouteLine = -1;
	iRoutePoint = -1;
	int i;
	Vector ViewAngles;
	g_Engine.GetViewAngles(ViewAngles);
	for (i = 0; i < routecount; i++)
	{
		if (route_line[i].enabled && i != lastroute)
		{
			float mindist = 150.f;
			for (unsigned int j = 0; j < route_line[i].count; j++)
			{
				dist = GetPointDistance(route_line[i].point[j].origin);
				if (dist < mindist)
				{
					if (abs(route_line[i].point[j].origin[2] - g_Local.vEye[2]) < 25)
					{
						GetPointAngles(route_line[i].point[j].origin, angles);
						pointangles = CalcTwoAngle(ViewAngles[1], angles[1]);
						if (GetPointVisible(route_line[i].point[j].origin) && pointangles <= 50)
						{
							if (j > 0 && j < route_line[i].count - 1)
							{
								dist1 = GetPointDistance(route_line[i].point[j + 1].origin);
								if (dist1 >= dist)
								{
									routeangle = CalcRouteAngle(route_line[i].point[j].origin, route_line[i].point[j + 1].origin);
									if (routeangle < minrouteangles)
									{
										minrouteangles = routeangle;
										route_direction = ROUTE_ASC;
										iRouteLine = i;
										iRoutePoint = j;
									}
								}
								else
								{
									routeangle = CalcRouteAngle(route_line[i].point[j].origin, route_line[i].point[j - 1].origin);
									if (routeangle < minrouteangles)
									{
										minrouteangles = routeangle;
										route_direction = ROUTE_DESC;
										iRouteLine = i;
										iRoutePoint = j;
									}
								}
							}
							else if (j == 0)
							{
								dist1 = GetPointDistance(route_line[i].point[j + 1].origin);
								if (dist1 >= dist)
								{
									routeangle = CalcRouteAngle(route_line[i].point[j].origin, route_line[i].point[j + 1].origin);
									if (routeangle < minrouteangles)
									{
										minrouteangles = routeangle;
										route_direction = ROUTE_ASC;
										iRouteLine = i;
										iRoutePoint = j;
									}
								}
							}
							else if (j == route_line[i].count - 1)
							{
								dist1 = GetPointDistance(route_line[i].point[j - 1].origin);
								if (dist1 >= dist)
								{
									routeangle = CalcRouteAngle(route_line[i].point[j].origin, route_line[i].point[j - 1].origin);
									if (routeangle < minrouteangles)
									{
										minrouteangles = routeangle;
										route_direction = ROUTE_DESC;
										iRouteLine = i;
										iRoutePoint = j;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (iRouteLine != -1 && iRoutePoint != -1)return;

	float angles1[3];
	minangles = 50;
	minrouteangles = 20;
	iRouteLine = -1;
	iRoutePoint = -1;

	for (i = 0; i < routecount; i++)
	{
		if (route_line[i].enabled && i != lastroute)
		{
			float mindist = 230;
			for (unsigned int j = 0; j < route_line[i].count; j++)
			{
				dist = GetPointDistance(route_line[i].point[j].origin);
				if (dist < mindist)
				{
					if (abs(route_line[i].point[j].origin[2] - g_Local.vEye[2]) < 25)
					{
						GetPointAngles(route_line[i].point[j].origin, angles);
						pointangles = CalcTwoAngle(ViewAngles[1], angles[1]);
						if (GetPointVisible(route_line[i].point[j].origin) && pointangles <= 50)
						{
							if (j > 0 && j < route_line[i].count - 1)
							{
								dist1 = GetPointDistance(route_line[i].point[j + 1].origin);
								if (dist1 >= dist)
								{
									GetPointAngles(route_line[i].point[j + 1].origin, angles1);
									routeangle = CalcTwoAngle(angles1[1], angles[1]);
									if (routeangle < minrouteangles && CalcTwoAngle(ViewAngles[1], angles1[1]) <= 50)
									{
										mindist = dist;
										minrouteangles = routeangle;
										route_direction = ROUTE_ASC;
										iRouteLine = i;
										iRoutePoint = j;
									}
								}
								else
								{
									GetPointAngles(route_line[i].point[j - 1].origin, angles1);
									routeangle = CalcTwoAngle(angles1[1], angles[1]);
									if (routeangle < minrouteangles && CalcTwoAngle(ViewAngles[1], angles1[1]) <= 50)
									{
										mindist = dist;
										minrouteangles = routeangle;
										route_direction = ROUTE_DESC;
										iRouteLine = i;
										iRoutePoint = j;
									}
								}
							}
							else if (j == 0)
							{
								dist1 = GetPointDistance(route_line[i].point[j + 1].origin);
								if (dist1 >= dist)
								{
									GetPointAngles(route_line[i].point[j + 1].origin, angles1);
									routeangle = CalcTwoAngle(angles1[1], angles[1]);
									if (routeangle < minrouteangles && CalcTwoAngle(ViewAngles[1], angles1[1]) <= 50)
									{
										mindist = dist;
										minrouteangles = routeangle;
										route_direction = ROUTE_ASC;
										iRouteLine = i;
										iRoutePoint = j;
									}
								}
							}
							else if (j == route_line[i].count - 1)
							{
								dist1 = GetPointDistance(route_line[i].point[j - 1].origin);
								if (dist1 >= dist)
								{
									GetPointAngles(route_line[i].point[j - 1].origin, angles1);
									routeangle = CalcTwoAngle(angles1[1], angles[1]);
									if (routeangle < minrouteangles && CalcTwoAngle(ViewAngles[1], angles1[1]) <= 50)
									{
										mindist = dist;
										minrouteangles = routeangle;
										route_direction = ROUTE_DESC;
										iRouteLine = i;
										iRoutePoint = j;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void AutoRoute::LoadRoute(char* sfilename)
{
	Record = false;
	char tfilename[256];
	if (sfilename[0] == '\0')
	{
		strcpy(levelname, g_Engine.pfnGetLevelName() + 5);
		levelname[strlen(levelname) - 4] = 0;
		sprintf(tfilename, "route/%s.route", levelname);
		sprintf(filename, "%s%s", hackdir, tfilename);
	}
	else
	{
		sprintf(tfilename, "route/%s.route", sfilename);
		sprintf(filename, "%s%s", hackdir, tfilename);
	}
	ZeroMemory(route_line, sizeof(route_line));
	ifstream ifs(filename);
	while (ifs.good())
	{
		char buf[1024] = { 0 };
		ifs.getline(buf, sizeof(buf));
		cout << buf << endl;
		int i, j;
		float f1 = 0, f2 = 0, f3 = 0;
		if (sscanf(buf, "route(%d)(%d):%f,%f,%f", &i, &j, &f1, &f2, &f3))
		{
			if (f1 != 0 && f2 != 0 && f3 != 0)
			{
				route_line[i].point[j].origin[0] = f1;
				route_line[i].point[j].origin[1] = f2;
				route_line[i].point[j].origin[2] = f3;
				route_line[i].enabled = true;
				route_line[i].count++;
			}
		}
	}
	ifs.close();

}

void AutoRoute::SaveRoute()
{
	char tfilename[256];
	strcpy(levelname, g_Engine.pfnGetLevelName() + 5);
	levelname[strlen(levelname) - 4] = 0;
	sprintf(tfilename, "route/%s.route", levelname);
	sprintf(filename, "%s%s", hackdir, tfilename);
	char routepoint[256];
	remove(filename);
	ofstream ofs(filename, ios::binary | ios::app);
	for (unsigned int i = 0; i < routecount; i++)
	{
		if (route_line[i].enabled)
		{
			for (unsigned int j = 0; j < route_line[i].count; j++)
			{
				sprintf(routepoint, "route(%d)(%d):%f,%f,%f", i, j, route_line[i].point[j].origin[0], route_line[i].point[j].origin[1], route_line[i].point[j].origin[2]);
				ofs << routepoint << (char)0x0D << (char)0x0A;
			}
		}
	}
	ofs.close();
}

void AutoRoute::AddPoint()
{
	if (iRouteLine > -1)
	{
		iRoutePoint++;
		if (iRoutePoint == routepointcount)
		{
			iRoutePoint = routepointcount - 1;
			Record = false;
			return;
		}
		VectorCopy(g_Local.vEye, route_line[iRouteLine].point[iRoutePoint].origin);
		route_line[iRouteLine].count++;
	}
}

void AutoRoute::ClearAllRoute()
{
	Record = false;
	ZeroMemory(route_line, sizeof(route_line));
}

void AutoRoute::ClearOneRoute(int index)
{
	if (index > -1 && index <= routecount)
	{
		Record = false;
		iRouteLine = index;
		iRoutePoint = -1;
		route_line[iRouteLine].enabled = false;
		route_line[iRouteLine].count = 0;
	}
}

void AutoRoute::RecordOneRouteStart(int index)
{
	if (index > -1 && index <= routecount)
	{
		iRouteLine = index;
		ClearOneRoute(iRouteLine);
		route_line[iRouteLine].enabled = true;
		route_line[iRouteLine].count = 0;
		Record = true;
	}
}

void AutoRoute::RecordAnyRouteStart()
{
	iRouteLine = routecount;
	for (unsigned int i = 0; i < routecount; i++)
	{
		if (!route_line[i].enabled)
		{
			iRouteLine = i;
			break;
		}
	}
	iRoutePoint = -1;
	route_line[iRouteLine].enabled = true;
	route_line[iRouteLine].count = 0;
	Record = true;
}

void AutoRoute::RecordRouteStop()
{
	Record = false;
}

void AutoRoute::FindAnyRoute(float pointdist, bool userand)
{
	iRouteLine = -1;
	iRoutePoint = -1;
	int iRouteLine1 = -1;
	int iRoutePoint1 = -1;
	float mindist = pointdist;
	for (unsigned int i = 0; i < routecount; i++)
	{
		if (route_line[i].enabled)
		{
			if (route_line[i].count > 12)
			{
				for (unsigned int j = 0; j < route_line[i].count; j++)
				{
					if (GetPointVisible(route_line[i].point[j].origin))
					{
						float dist = GetPointDistance(route_line[i].point[j].origin);
						if (dist < mindist && abs(route_line[i].point[j].origin[2] - g_Local.vEye[2]) < 25)
						{
							mindist = dist;
							iRouteLine1 = i;
							iRoutePoint1 = j;
						}
					}
				}
			}
		}
	}
	if (userand)
	{
		int iRouteLine2 = -1;
		int iRoutePoint2 = -1;
		mindist = pointdist;
		for (unsigned int i = 0; i < routecount; i++)
		{
			if (i == iRouteLine1)continue;
			if (route_line[i].enabled)
			{
				if (route_line[i].count > 12)
				{
					for (unsigned int j = 0; j < route_line[i].count; j++)
					{
						if (GetPointVisible(route_line[i].point[j].origin))
						{
							float dist = GetPointDistance(route_line[i].point[j].origin);
							if (dist < mindist && abs(route_line[i].point[j].origin[2] - g_Local.vEye[2]) < 25)
							{
								mindist = dist;
								iRouteLine2 = i;
								iRoutePoint2 = j;
							}
						}
					}
				}
			}
		}
		if (iRouteLine2 == -1)
		{
			iRouteLine2 = iRouteLine1;
			iRoutePoint2 = iRoutePoint1;
		}
		int iRouteLine3 = -1;
		int iRoutePoint3 = -1;
		mindist = pointdist;
		for (unsigned int i = 0; i < routecount; i++)
		{
			if (i == iRouteLine1 || i == iRouteLine2)continue;
			if (route_line[i].enabled)
			{
				if (route_line[i].count > 12)
				{
					for (unsigned int j = 0; j < route_line[i].count; j++)
					{
						if (GetPointVisible(route_line[i].point[j].origin))
						{
							float dist = GetPointDistance(route_line[i].point[j].origin);
							if (dist < mindist && abs(route_line[i].point[j].origin[2] - g_Local.vEye[2]) < 25)
							{
								mindist = dist;
								iRouteLine3 = i;
								iRoutePoint3 = j;
							}
						}
					}
				}
			}
		}
		if (iRouteLine3 == -1)
		{
			iRouteLine3 = iRouteLine1;
			iRoutePoint3 = iRoutePoint1;
		}
		srand((unsigned)time(NULL));
		int randnum = rand() % 3 + 1;
		if (randnum == 1)
		{
			iRouteLine = iRouteLine1;
			iRoutePoint = iRoutePoint1;
		}
		else if (randnum == 2)
		{
			iRouteLine = iRouteLine2;
			iRoutePoint = iRoutePoint2;
		}
		else
		{
			iRouteLine = iRouteLine3;
			iRoutePoint = iRoutePoint3;
		}
	}
	else
	{
		iRouteLine = iRouteLine1;
		iRoutePoint = iRoutePoint1;
	}
	if (iRouteLine > -1 && iRoutePoint > -1)
	{
		if (iRoutePoint <= route_line[iRouteLine].count - iRoutePoint)
			route_direction = ROUTE_ASC;
		else
			route_direction = ROUTE_DESC;
		route_mode = ROUTE_START;
	}
	else
	{
		route_mode = ROUTE_NONE;
	}
}

void AutoRoute::RouteTurn()
{
	if (route_mode == ROUTE_FROM || route_mode == ROUTE_START)
	{
		if (route_direction == ROUTE_ASC)
		{
			route_direction = ROUTE_DESC;
			if (iRoutePoint > 0)
				iRoutePoint--;
		}
		else if (route_direction == ROUTE_DESC)
		{
			route_direction = ROUTE_ASC;
			if (iRoutePoint < route_line[iRouteLine].count - 1)
				iRoutePoint++;
		}
		if (route_line[iRouteLine].point[iRoutePoint].origin[2] - g_Local.vEye[2] < 25 && route_line[iRouteLine].point[iRoutePoint].origin[2] - g_Local.vEye[2] > -100 && GetPointVisible(route_line[iRouteLine].point[iRoutePoint].origin))
			route_mode = ROUTE_START;
		else
		{
			iRouteLine = -1;
			iRoutePoint = -1;
			route_mode = ROUTE_NONE;
			FindAnyRoute(10000, false);
		}
	}
	else
	{
		iRouteLine = -1;
		iRoutePoint = -1;
		route_mode = ROUTE_NONE;
		FindAnyRoute(10000, false);
	}
}

void func_route_load()
{
	if (cmd.argS(1).empty())
	{
		autoroute.LoadRoute("");
	}
	else
	{
		char* sfilename = cmd.argC(1);
		autoroute.LoadRoute(sfilename);
	}
}

void func_route_save()
{
	autoroute.SaveRoute();
}

void func_route_add_point()
{
	autoroute.AddPoint();
}

void func_route_record_start()
{
	autoroute.RecordAnyRouteStart();
}

void func_route_record_line()
{
	int index = cmd.argI(1) - 1;
	autoroute.RecordOneRouteStart(index);
}

void func_route_clear_line()
{
	int index = cmd.argI(1) - 1;
	autoroute.ClearOneRoute(index);
}

void func_route_clear()
{
	autoroute.ClearAllRoute();
}

void func_route_record_stop()
{
	autoroute.RecordRouteStop();
}

void func_route_record_add()
{
	autoroute.AddPoint();
}

void RouteCommands()
{
	cmd.AddCommand("route_load", func_route_load);
	cmd.AddCommand("route_save", func_route_save);
	cmd.AddCommand("route_record_start", func_route_record_start);
	cmd.AddCommand("route_record_line", func_route_record_line);
	cmd.AddCommand("route_clear_line", func_route_clear_line);
	cmd.AddCommand("route_clear", func_route_clear);
	cmd.AddCommand("route_record_stop", func_route_record_stop);
	cmd.AddCommand("route_record_add", func_route_record_add);
}

float MySpeed()
{
	return sqrt(g_Local.vVelocity[0] * g_Local.vVelocity[0] + g_Local.vVelocity[1] * g_Local.vVelocity[1]);
}

bool InMove(usercmd_s* cmd)
{
	if (cmd->buttons & IN_FORWARD || cmd->buttons & IN_BACK || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT)
		return true;
	return false;
}

void RouteDirection(usercmd_s* cmd)
{
	static int iTurnMove = false;
	static int iJumpMove = false;
	if (cvar.route_auto && g_Local.bAlive && !IsFreezePeriod())
	{
		if (InMove(cmd) && MySpeed() < 30)
		{
			iJumpMove++;
			if (iJumpMove >= cvar.route_jump_step)
			{
				if (cvar.route_jump)
					cmd->buttons |= IN_JUMP;
				iJumpMove = false;
			}
			iTurnMove++;
			if (iTurnMove >= cvar.route_direction_step)
			{
				if (cvar.route_direction)
				{
					Vector ViewAngles;
					g_Engine.GetViewAngles(ViewAngles);
					if (g_Local.weapon.m_iClip)
					{
						if (!iTargetRage)
						{
							ViewAngles[1] += 90;
							g_Engine.SetViewAngles(ViewAngles);
							autoroute.RouteTurn();
						}
					}
					else
					{
						ViewAngles[1] += 90;
						g_Engine.SetViewAngles(ViewAngles);
						autoroute.RouteTurn();
					}
				}
				iTurnMove = false;
			}
		}
		else
		{
			iTurnMove = false;
			iJumpMove = false;
		}
	}
}

void DrawRouteLine()
{
	float WindowBorderSize = ImGui::GetStyle().WindowBorderSize;
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::Begin("Routeline", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	{
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiSetCond_Always);

		if (CheckDraw() && g_pEngine->GetMaxClients())
		{
			for (unsigned int i = 0; i < autoroute.routecount; i++)
			{
				if (autoroute.route_line[i].enabled)
				{
					for (unsigned int j = 0; j < autoroute.route_line[i].count; j++)
					{
						if (cvar.route_draw && g_Local.bAlive && cvar.rage_active)
						{
							float dist = GetPointDistance(autoroute.route_line[i].point[j].origin);
							if (dist < 800)
							{
								if (j == 0 || j == autoroute.route_line[i].count - 1)
								{
									vec3_t vecPoint;
									vecPoint = autoroute.route_line[i].point[j].origin; 
									vecPoint[2] -= 37;
									float vecScreen[2];
									if (WorldToScreen(vecPoint, vecScreen))
									{
										char str[10];
										sprintf(str, "%d", i + 1);
										int label_size = ImGui::CalcTextSize(str, NULL, true).x;
										int ScreenX = vecScreen[0], ScreenY = vecScreen[1];
										ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)ScreenX - label_size / 2 - 2, (float)ScreenY - 23 }, { (float)ScreenX - label_size / 2 + label_size + 4, (float)ScreenY - 9 }, black(), cvar.visual_rounding);
										ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)ScreenX - label_size / 2 - 3, (float)ScreenY - 24 }, { (float)ScreenX - label_size / 2 + label_size + 3, (float)ScreenY - 10 }, wheel1(), cvar.visual_rounding);
										ImGui::GetCurrentWindow()->DrawList->AddText({ (float)ScreenX - label_size / 2, (float)ScreenY - 25 }, white(), str);
									}
									if (WorldToScreen(vecPoint, vecScreen))
									{
										ImGui::GetCurrentWindow()->DrawList->AddRect({ vecScreen[0] - 1, vecScreen[1] - 1 }, { vecScreen[0] + 4, vecScreen[1] + 4 }, black(), cvar.visual_rounding);
										ImGui::GetCurrentWindow()->DrawList->AddRect({ vecScreen[0] - 2, vecScreen[1] - 2 }, { vecScreen[0] + 3, vecScreen[1] + 3 }, wheel2(), cvar.visual_rounding);
									}
								}
								if (j > 0 && j % 1 == 0)
								{
									vec3_t vecBegin, vecEnd;
									vecBegin = autoroute.route_line[i].point[j - 1].origin;
									vecBegin[2] -= 37;
									vecEnd = autoroute.route_line[i].point[j].origin;
									vecEnd[2] -= 37;
									if (DrawVisuals)
									{
										int beamindex = g_Engine.pEventAPI->EV_FindModelIndex("sprites/laserbeam.spr");
										g_Engine.pEfxAPI->R_BeamPoints(vecBegin, vecEnd, beamindex, 0.02f, 1.f, 0.5, 1, 0, 0, 0, cvar.color_green, cvar.color_blue, cvar.color_red);
									}
									float vecScreen[2], vecScreen2[2];
									if (WorldToScreen(vecBegin, vecScreen) && WorldToScreen(vecEnd, vecScreen2))
									{
										ImGui::GetCurrentWindow()->DrawList->AddLine({ vecScreen[0] + 1, vecScreen[1] + 1 }, { vecScreen2[0] + 1, vecScreen2[1] + 1 }, black());
										ImGui::GetCurrentWindow()->DrawList->AddLine({ vecScreen[0], vecScreen[1] }, { vecScreen2[0], vecScreen2[1] }, wheel1());
									}
								}
							}
						}
					}
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::GetStyle().WindowBorderSize = WindowBorderSize;
	}
}

void Route(usercmd_s* cmd)
{
	autoroute.CreateMove(cmd);
}

void ContinueRoute()
{
	if (cvar.route_auto)
		autoroute.bContinueAutoRoute = true;
	else
		autoroute.bContinueAutoRoute = false;

	autoroute.LoadRoute("");
}
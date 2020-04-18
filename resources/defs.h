#ifndef DEFS_H
#define DEFS_H

#ifndef VectorSubtract
#define VectorSubtract(a,b,c) {(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];(c)[2]=(a)[2]-(b)[2];}
#endif

#ifndef VectorAdd
#define VectorAdd(a,b,c) {(c)[0]=(a)[0]+(b)[0];(c)[1]=(a)[1]+(b)[1];(c)[2]=(a)[2]+(b)[2];}
#endif

#ifndef VectorCopy
#define VectorCopy(a,b) {(b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];}
#endif

#ifndef VectorClear
#define VectorClear(a) { a[0]=0.0;a[1]=0.0;a[2]=0.0;}
#endif

#ifndef DotProd
#define DotProd(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#endif

#ifndef VectorScale
#define VectorScale(a,b,c) {(c)[0]=(b)*(a)[0];(c)[1]=(b)*(a)[1];(c)[2]=(b)*(a)[2];}
#endif

#ifndef POW
#define POW(x) ((x)*(x))
#endif

#ifndef VectorDistance
#define VectorDistance(a,b) sqrt(POW((a)[0]-(b)[0])+POW((a)[1]-(b)[1])+POW((a)[2]-(b)[2]))
#endif

#ifndef VectorLength
#define VectorLength(a) sqrt(POW((a)[0])+POW((a)[1])+POW((a)[2]))
#endif

// player data iuser3
#define PLAYER_CAN_SHOOT		(1<<0)
#define PLAYER_FREEZE_TIME_OVER		(1<<1)
#define PLAYER_IN_BOMB_ZONE		(1<<2)
#define PLAYER_HOLDING_SHIELD		(1<<3)

#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / M_PI) )
#define DEG2RAD( x ) ( (float)(x) * (float)(M_PI / 180.f) )
#define VectorMul(vec,num,res){(res)[0]=(vec)[0]*(num);(res)[1]=(vec)[1]*(num);(res)[2]=(vec)[2]*(num);}

#define NUMBLOOPS 50.0f
#define TIMEALIVE 3.00f
#define OLDDAMPER 1.75f 
#define NEWDAMPER 0.75f
#define SVGRAVITY 3.75f
#define FLOORSTOP 0.20f

#define PM_DEAD_VIEWHEIGHT -8
#define PM_VEC_DUCK_VIEW 12
#define PM_VEC_VIEW	17

#define OBS_NONE				0
#define OBS_CHASE_LOCKED		1
#define OBS_CHASE_FREE			2
#define OBS_ROAMING				3
#define OBS_IN_EYE				4
#define OBS_MAP_FREE			5
#define OBS_MAP_CHASE			6

typedef float BoneMatrix_t[MAXSTUDIOBONES][3][4];

#define CHECKGUNOFFSET(a,b) if(!strcmp(a,gunname)) return b;

#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C

#define GAITSEQUENCE_STAND 1
#define GAITSEQUENCE_DUCK 2
#define GAITSEQUENCE_WALK 3
#define GAITSEQUENCE_RUNNING 4
#define GAITSEQUENCE_DUCKMOVE 5
#define GAITSEQUENCE_JUMP 6

#define DotDist(a,b) sqrt(POW((a)[0]-(b)[0])+POW((a)[1]-(b)[1])+POW((a)[2]-(b)[2]))

#define MENUBACK			    1
#define TARGET			        2
#define RAGE			        3
#define LEGIT			        4
#define KNIFE			        5
#define SETTINGS			    6
#define MODEL			        7
#define VISUAL			        8
#define BUNNY			        9
#define WAYPOINT			    10
#define COLOR			        11
#define BACKDROP			    12

#define PLAYER1 31
#define PLAYER2 32
#define PLAYER3 33
#define PLAYER4 34
#define PLAYER5 35
#define PLAYER6 36
#define PLAYER7 37
#define PLAYER8 38

#define BACKPACK1 39
#define BACKPACK2 40
#define BACKPACK3 41
#define BACKPACK4 42
#define BACKPACK5 43

#define THIGHPACK1 44
#define THIGHPACK2 45
#define THIGHPACK3 46
#define THIGHPACK4 47
#define THIGHPACK5 48

#define AK471 49
#define AK472 50
#define AK473 51
#define AK474 52
#define AK475 53
#define AK476 54
#define AK477 55
#define AK478 56
#define AK479 57
#define AK4710 58
#define AK4711 59

#define KEVLAR 60

#define AUG1 61
#define AUG2 62
#define AUG3 63
#define AUG4 64
#define AUG5 65
#define AUG6 66

#define AWP1 67
#define AWP2 68
#define AWP3 69
#define AWP4 70
#define AWP5 71
#define AWP6 72
#define AWP7 73
#define AWP8 74
#define AWP9 75
#define AWP10 76
#define AWP11 77
#define AWP12 78
#define AWP13 79

#define C41 80
#define C42 81
#define C43 82
#define C44 83
#define C45 84
#define C46 85
#define C47 86
#define C48 87
#define C49 88
#define C410 89
#define C411 91

#define DEAGLE1 92
#define DEAGLE2 93
#define DEAGLE3 94
#define DEAGLE4 95
#define DEAGLE5 96
#define DEAGLE6 97
#define DEAGLE7 98
#define DEAGLE8 99
#define DEAGLE9 100

#define ELITE1 101
#define ELITE2 102
#define ELITE3 103
#define ELITE4 104
#define ELITE5 105
#define ELITE6 106

#define FAMAS1 107
#define FAMAS2 108
#define FAMAS3 109
#define FAMAS4 110

#define FIVESEVEN1 111
#define FIVESEVEN2 112
#define FIVESEVEN3 113
#define FIVESEVEN4 114
#define FIVESEVEN5 115
#define FIVESEVEN6 116
#define FIVESEVEN7 117

#define FLASHBANG1 118
#define FLASHBANG2 119
#define FLASHBANG3 120
#define FLASHBANG4 121
#define FLASHBANG5 122
#define FLASHBANG6 123
#define FLASHBANG7 124
#define FLASHBANG8 125
#define FLASHBANG9 126
#define FLASHBANG10 127
#define FLASHBANG11 128
#define FLASHBANG12 129

#define G3SG11 130
#define G3SG12 131
#define G3SG13 132
#define G3SG14 133
#define G3SG15 134
#define G3SG16 135
#define G3SG17 136
#define G3SG18 137
#define G3SG19 138
#define G3SG110 139
#define G3SG111 140

#define GALIL1 141
#define GALIL2 142
#define GALIL3 143
#define GALIL4 144

#define GLOCK181 145
#define GLOCK182 146
#define GLOCK183 147
#define GLOCK184 148
#define GLOCK185 149
#define GLOCK186 150
#define GLOCK187 151
#define GLOCK188 152
#define GLOCK189 153
#define GLOCK1810 154
#define GLOCK1811 155

#define HEGRENADE1 156
#define HEGRENADE2 157
#define HEGRENADE3 158
#define HEGRENADE4 159
#define HEGRENADE5 160
#define HEGRENADE6 161
#define HEGRENADE7 162
#define HEGRENADE8 163
#define HEGRENADE9 164
#define HEGRENADE10 165
#define HEGRENADE11 166
#define HEGRENADE12 167

#define KNIFE1 168
#define KNIFE2 169
#define KNIFE3 170

#define M31 171
#define M32 172
#define M33 173
#define M34 174
#define M35 175
#define M36 176
#define M37 177

#define M4A11 178
#define M4A12 179
#define M4A13 180
#define M4A14 181
#define M4A15 182
#define M4A16 183
#define M4A17 184
#define M4A18 185
#define M4A19 186

#define M2491 187
#define M2492 188
#define M2493 189
#define M2494 190
#define M2495 191
#define M2496 192
#define M2497 193
#define M2498 194
#define M2499 195
#define M24910 196
#define M24911 197
#define M24912 198

#define MAC101 199
#define MAC102 200
#define MAC103 201
#define MAC104 202
#define MAC105 203

#define MP51 204
#define MP52 205
#define MP53 206
#define MP54 207
#define MP55 208
#define MP56 209
#define MP57 210
#define MP58 211
#define MP59 212
#define MP510 213

#define P901 214
#define P902 215
#define P903 216
#define P904 217
#define P905 218
#define P906 219
#define P907 220
#define P908 221

#define P2281 222
#define P2282 223
#define P2283 224
#define P2284 225
#define P2285 226
#define P2286 227
#define P2287 228
#define P2288 229
#define P2289 230

#define SCOUT1 231
#define SCOUT2 232
#define SCOUT3 233
#define SCOUT4 234
#define SCOUT5 235
#define SCOUT6 236
#define SCOUT7 237
#define SCOUT8 238

#define SG5501 239
#define SG5502 240
#define SG5503 241
#define SG5504 242
#define SG5505 243
#define SG5506 244
#define SG5507 245
#define SG5508 246
#define SG5509 247

#define SG5521 248
#define SG5522 249
#define SG5523 250
#define SG5524 251
#define SG5525 252
#define SG5526 253
#define SG5527 254
#define SG5528 255

#define SHIELD1 256
#define SHIELD2 257
#define SHIELD3 258
#define SHIELD4 259
#define SHIELD5 260
#define SHIELD6 261
#define SHIELD7 262
#define SHIELD8 263
#define SHIELD9 264
#define SHIELD10 265
#define SHIELD11 266
#define SHIELD12 267
#define SHIELD13 268
#define SHIELD14 269
#define SHIELD15 270
#define SHIELD16 271
#define SHIELD17 272
#define SHIELD18 273
#define SHIELD19 274
#define SHIELD20 275
#define SHIELD21 276
#define SHIELD22 277
#define SHIELD23 278
#define SHIELD24 279
#define SHIELD25 280
#define SHIELD26 281
#define SHIELD27 282
#define SHIELD28 283
#define SHIELD29 284
#define SHIELD30 285
#define SHIELD31 286
#define SHIELD32 287
#define SHIELD33 288
#define SHIELD34 289
#define SHIELD35 290
#define SHIELD36 291
#define SHIELD37 292
#define SHIELD38 293
#define SHIELD39 294
#define SHIELD40 295
#define SHIELD41 296
#define SHIELD42 297
#define SHIELD43 298
#define SHIELD44 299
#define SHIELD45 300
#define SHIELD46 301
#define SHIELD47 302

#define SMOKEGRENADE1 303
#define SMOKEGRENADE2 304
#define SMOKEGRENADE3 305
#define SMOKEGRENADE4 306
#define SMOKEGRENADE5 307
#define SMOKEGRENADE6 308
#define SMOKEGRENADE7 309
#define SMOKEGRENADE8 310
#define SMOKEGRENADE9 311
#define SMOKEGRENADE10 312
#define SMOKEGRENADE11 313
#define SMOKEGRENADE12 314

#define TMP1 315
#define TMP2 316
#define TMP3 317
#define TMP4 318
#define TMP5 319
#define TMP6 320
#define TMP7 321
#define TMP8 322
#define TMP9 323
#define TMP10 324
#define TMP11 325

#define UMP451 326
#define UMP452 327
#define UMP453 328
#define UMP454 329
#define UMP455 330
#define UMP456 331

#define USP1 332
#define USP2 333
#define USP3 334
#define USP4 335
#define USP5 336
#define USP6 337
#define USP7 338
#define USP8 339
#define USP9 340
#define USP10 341

#define XM10141 342
#define XM10142 343
#define XM10143 344
#define XM10144 345
#define XM10145 346
#define XM10146 347
#define XM10147 348
#define XM10148 349
#define XM10149 350

#define HANDS1 351
#define HANDS2 352
#define HANDS3 353
#define HANDS4 354
#define HANDS5 355
#define HANDS6 356
#define HANDS7 357
#define HANDS8 358
#define HANDS9 359
#define HANDS10 360

#define HANDS11 361
#define HANDS12 362
#define HANDS13 363
#define HANDS14 364
#define HANDS15 365
#define HANDS16 366
#define HANDS17 367
#define HANDS18 368
#define HANDS19 369
#define HANDS20 370

#define HANDS21 371
#define HANDS22 372
#define HANDS23 373
#define HANDS24 374
#define HANDS25 375
#define HANDS26 376
#define HANDS27 377
#define HANDS28 378
#define HANDS29 379
#define HANDS30 380

#define HANDS31 381
#define HANDS32 382
#define HANDS33 383
#define HANDS34 384
#define HANDS35 385
#define HANDS36 386
#define HANDS37 387
#define HANDS38 388
#define HANDS39 389
#define HANDS40 390

#define HANDS41 391
#define HANDS42 392
#define HANDS43 393
#define HANDS44 394
#define HANDS45 395
#define HANDS46 396
#define HANDS47 397
#define HANDS48 398
#define HANDS49 399
#define HANDS50 400

#define HANDS51 401
#define HANDS52 402
#define HANDS53 403
#define HANDS54 404
#define HANDS55 405
#define HANDS56 406
#define HANDS57 407
#define HANDS58 408
#define HANDS59 409
#define HANDS60 410

#define HANDS61 411
#define HANDS62 412
#define HANDS63 413
#define HANDS64 414
#define HANDS65 415
#define HANDS66 416
#define HANDS67 417
#define HANDS68 418
#define HANDS69 419
#define HANDS70 420

#define HANDS71 421
#define HANDS72 422
#define HANDS73 423
#define HANDS74 424
#define HANDS75 425
#define HANDS76 426
#define HANDS77 427
#define HANDS78 428
#define HANDS79 429
#define HANDS80 430

#define HANDS81 431
#define HANDS82 432
#define HANDS83 433
#define HANDS84 434
#define HANDS85 435
#define HANDS86 436
#define HANDS87 437
#define HANDS88 438
#define HANDS89 439
#define HANDS90 440

#define HANDS91 441
#define HANDS92 442
#define HANDS93 443
#define HANDS94 444
#define HANDS95 445
#define HANDS96 446
#define HANDS97 447
#define HANDS98 448
#define HANDS99 449
#define HANDS100 450

#define HANDS101 451
#define HANDS102 452
#define HANDS103 453
#define HANDS104 454
#define HANDS105 455
#define HANDS106 456
#define HANDS107 457
#define HANDS108 458
#define HANDS109 459
#define HANDS110 460

#define HANDS111 461
#define HANDS112 462
#define HANDS113 463
#define HANDS114 464

#define SHELL1 465
#define SHELL2 466
#define SHELL3 467
#define SHELL4 468

#define CHICKEN 469

#define VIPBODY 470
#define VIPHEAD 471
#define VIPBODY2 472
#define VIPHEAD2 473

#define ARCTICBODY 474
#define ARCTICHEAD 475

#define GIGNBODY 476
#define GIGNHEAD 477

#define GSG9BODY 478
#define GSG9HEAD 479

#define GUERILLABODY 480
#define GUERILLAHEAD 481

#define LEETBODY 482
#define LEETHEAD 483

#define SASBODY 484
#define SASHEAD 485

#define TERRORBODY 486
#define TERRORHEAD 487

#define URBANBODY 488
#define URBANHEAD 489
#define URBANCHROME 490

#endif

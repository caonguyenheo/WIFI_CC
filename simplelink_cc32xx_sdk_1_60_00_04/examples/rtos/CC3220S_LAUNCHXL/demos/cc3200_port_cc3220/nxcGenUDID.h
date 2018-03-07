/*
* Copyright (C) 2014 CVision
*
* This unpublished material is proprietary to CVision.
* All rights reserved. The methods and
* techniques described herein are considered trade secrets
* and/or confidential. Reproduction or distribution, in whole
* or in part, is forbidden except by express written permission
* of CVision.
*/

#ifndef NXCGENUDID_H_
#define NXCGENUDID_H_

int inxcGenUDID(unsigned short vendor_id, unsigned short chrTypeNum, char* usModelNum, char* strMAC, char* strUDID);
int unit_test_udid(void);

#endif /* NXCGENUDID_H_ */

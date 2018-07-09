#ifndef SELFNEGO_H_
#define SELFNEGO_H_

#include "NetState.h"

#include <dds\DdsDcpsInfrastructureC.h>
#include <dds\DdsDcpsCoreC.h>
#include <dds\DdsDcpsPublicationC.h>
#include <dds\DdsDcpsSubscriptionC.h>
#include <dds\DCPS\StaticIncludes.h>
#include <dds\DCPS\Service_Participant.h>
#include <dds\DCPS\Marked_Default_Qos.h>
#include <dds\DCPS\transport\framework\TransportRegistry.h>
#include <dds\DCPS\transport\framework\TransportExceptions.h>

//所有的阈值都在这里定义
#define DELAY_THRESHOLD_1 0
#define DELAY_THRESHOLD_2 100
#define DELAY_THRESHOLD_3 200
#define	PACKETLOSS_THRESHOLD_1 0
#define	PACKETLOSS_THRESHOLD_2 15
#define	PACKETLOSS_THRESHOLD_3 30

#include <iostream>
#include <string>
#include <cstring>

class SelfNego
{
public:
	static SelfNego *getInstance()
	{
		if (s_instance == NULL)
		{
			s_instance = new SelfNego();
		}
		return s_instance;
	}
	void getNetState();
	int getQoS();
	DDS::DataReaderQos getReaderQos(char input[], int &isUdp);
	DDS::DataWriterQos getWriterQos(char input[], int &isUdp);

private:
	SelfNego(void);
	~SelfNego(void);
	static SelfNego *s_instance;
	NetState *netState;
	DDS::DataReaderQos reader;  //1
	DDS::DataWriterQos writer;  //2
	int isReader;
	char ip[20];
};

#endif
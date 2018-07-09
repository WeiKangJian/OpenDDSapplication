#pragma once



#include <cstring>

#include <dds\DdsDcpsInfrastructureC.h>
#include <dds\DdsDcpsPublicationC.h>
#include <dds/DdsDcpsSubscriptionC.h>
#include <dds\DCPS\StaticIncludes.h>
#include <dds\DCPS\Service_Participant.h>
#include <dds\DCPS\Marked_Default_Qos.h>


class NetState {
public:
	NetState();
	~NetState();
	void test(char ip[]);
	void setBandWidth(double input);
	double getBandWidth();
	/*void setThroughput(double input);
	double getThroughput();
	void setRatio(double input);
	double getRatio();*/
	void setDelay(double input);
	double getDelay();
	void setPacketLoss(double input);
	double getPacketLoss();

private:
	double bandWidth;  //������λbps
	//double throughput; //����������λbps
	//double ratio;      //������
	double delay;      //ʱ�ӣ���λms
	double packetLoss; //������
};

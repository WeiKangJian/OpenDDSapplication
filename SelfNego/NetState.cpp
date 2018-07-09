#pragma

#include "NetState.h"
#include<stdlib.h>


NetState::NetState() {
}

NetState::~NetState() {
}

void NetState::test(char ip[]) {
	char cmd[100] = "nestat.exe ";
	strcat_s(cmd, ip);
	FILE *pf = _popen(cmd, "r");
	char res[1000] = { 0 };
	fread(res, 1000, 1, pf);
	_pclose(pf);
	char re[4][100] = { 0 };
	int k = 0;
	int j = 0;
	for (int i = 0; i < 1000; i++) {
		if (res[i] == '\n' || res[i] == ' ' || res[i] == 37) {
			j = 0;
			k++;
			if (res[i] == 37)
				i++;
			continue;
		}
		if (k == 4)
			break;
		re[k][j++] = res[i];
	}
	setDelay(atof(re[1]));
	setPacketLoss(atof(re[2]));
	setBandWidth(atof(re[3]));
}

void NetState::setBandWidth(double input)
{
	bandWidth = input;
}

double NetState::getBandWidth()
{
	return bandWidth;
}

//void NetState::setThroughput(double input)
//{
//	throughput = input;
//}
//
//double NetState::getThroughput()
//{
//	return throughput;
//}
//
//void NetState::setRatio(double input)
//{
//	ratio = input;
//}
//
//double NetState::getRatio()
//{
//	return ratio;
//}

void NetState::setDelay(double input)
{
	delay = input;
}

double NetState::getDelay()
{
	return delay;
}

void NetState::setPacketLoss(double input)
{
	packetLoss = input;
}

double NetState::getPacketLoss()
{
	return packetLoss;
}
